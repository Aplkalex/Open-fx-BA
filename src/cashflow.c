/**
 * Open fx-BA: TI BA II Plus Clone
 * cashflow.c - Cash Flow analysis implementation
 */

#include "cashflow.h"
#include "config.h"
#include <math.h>
#include <string.h>

/* ============================================================
 * Cash Flow List Management
 * ============================================================ */

void cf_init(CashFlowList *cf) {
  cf->CF0 = 0.0;
  cf->count = 0;
  memset(cf->flows, 0, sizeof(cf->flows));
}

void cf_set_cf0(CashFlowList *cf, double amount) { cf->CF0 = amount; }

int cf_add(CashFlowList *cf, double amount, int frequency) {
  if (cf->count >= MAX_CASH_FLOWS) {
    return -1;
  }

  /* Default frequency is 1 */
  if (frequency < 1)
    frequency = 1;
  if (frequency > 9999)
    frequency = 9999; /* TI limit */

  cf->flows[cf->count].amount = amount;
  cf->flows[cf->count].frequency = frequency;
  cf->count++;

  return cf->count - 1;
}

void cf_update(CashFlowList *cf, int index, double amount, int frequency) {
  if (index < 0 || index >= cf->count)
    return;

  if (frequency < 1)
    frequency = 1;
  if (frequency > 9999)
    frequency = 9999;

  cf->flows[index].amount = amount;
  cf->flows[index].frequency = frequency;
}

void cf_delete(CashFlowList *cf, int index) {
  if (index < 0 || index >= cf->count)
    return;

  /* Shift remaining flows down */
  for (int i = index; i < cf->count - 1; i++) {
    cf->flows[i] = cf->flows[i + 1];
  }

  cf->count--;
}

int cf_total_periods(CashFlowList *cf) {
  int total = 0;
  for (int i = 0; i < cf->count; i++) {
    total += cf->flows[i].frequency;
  }
  return total;
}

/* ============================================================
 * NPV Calculation (OPTIMIZED)
 * ============================================================ */

double cf_npv(CashFlowList *cf, double rate) {
  /*
   * NPV = CF0 + sum(CFj / (1+r)^t)
   * 
   * OPTIMIZED: Use iterative discount factor multiplication instead
   * of calling pow() for each period. This is O(n) multiplications
   * instead of O(n) pow() calls (which are expensive).
   */

  double npv = cf->CF0;
  double discountFactor = 1.0;
  double onePlusRate = 1.0 + rate;

  for (int i = 0; i < cf->count; i++) {
    double amount = cf->flows[i].amount;
    int freq = cf->flows[i].frequency;

    for (int f = 0; f < freq; f++) {
      discountFactor /= onePlusRate;  /* Equivalent to 1/(1+r)^period */
      npv += amount * discountFactor;
    }
  }

  return npv;
}

/* ============================================================
 * IRR Calculation (Newton-Raphson) - OPTIMIZED
 * ============================================================ */

/*
 * Compute NPV and its derivative in a single pass.
 * This halves the number of iterations through the cash flow list.
 * 
 * Derivative: d(NPV)/dr = sum( -t * CFj / (1+r)^(t+1) )
 */
static void cf_npv_and_derivative(CashFlowList *cf, double rate, 
                                   double *npv, double *dnpv) {
  *npv = cf->CF0;
  *dnpv = 0.0;
  
  double onePlusRate = 1.0 + rate;
  double discountFactor = 1.0;
  int period = 0;

  for (int i = 0; i < cf->count; i++) {
    double amount = cf->flows[i].amount;
    int freq = cf->flows[i].frequency;

    for (int f = 0; f < freq; f++) {
      period++;
      discountFactor /= onePlusRate;
      *npv += amount * discountFactor;
      /* Derivative: -period * CF / (1+r)^(period+1) = -period * CF * discountFactor / (1+r) */
      *dnpv -= (double)period * amount * discountFactor / onePlusRate;
    }
  }
}

double cf_irr(CashFlowList *cf, int *errorCode) {
  *errorCode = ERR_NONE;

  /* Check if we have any cash flows */
  if (cf->count == 0) {
    *errorCode = ERR_INVALID_INPUT;
    return 0.0;
  }

  /* Check for sign changes (necessary for IRR to exist) */
  int hasPositive = (cf->CF0 > 0);
  int hasNegative = (cf->CF0 < 0);

  for (int i = 0; i < cf->count; i++) {
    if (cf->flows[i].amount > 0)
      hasPositive = 1;
    if (cf->flows[i].amount < 0)
      hasNegative = 1;
  }

  if (!hasPositive || !hasNegative) {
    *errorCode = ERR_NO_SOLUTION;
    return 0.0;
  }

  /* Newton-Raphson iteration with optimized function evaluation */
  double rate = INITIAL_GUESS;
  double f, df;

  for (int iter = 0; iter < MAX_ITERATIONS; iter++) {
    /* Compute NPV and derivative in single pass */
    cf_npv_and_derivative(cf, rate, &f, &df);

    /* Check for convergence */
    if (fabs(f) < TOLERANCE) {
      return rate;
    }

    if (fabs(df) < 1e-15) {
      /* Derivative too small */
      break;
    }

    double newRate = rate - f / df;

    /* Bound the rate */
    if (newRate < -0.999)
      newRate = -0.999;
    if (newRate > 10.0)
      newRate = 10.0;  /* 1000% max */

    if (fabs(newRate - rate) < TOLERANCE) {
      return newRate;
    }

    rate = newRate;
  }

  /* Failed to converge - might have multiple IRRs */
  *errorCode = ERR_IRR_MULTIPLE;
  return 0.0;
}

/* ============================================================
 * NFV Calculation (Pro only) - OPTIMIZED
 * ============================================================ */

double cf_nfv(CashFlowList *cf, double rate) {
  /*
   * NFV = NPV * (1 + r)^n
   * 
   * OPTIMIZED: Compute directly without separate NPV call
   * NFV = CF0*(1+r)^n + CF1*(1+r)^(n-1) + ... + CFn
   */

  int n = cf_total_periods(cf);
  double onePlusRate = 1.0 + rate;
  
  /* Start with CF0 compounded n periods */
  double nfv = cf->CF0 * pow(onePlusRate, (double)n);
  
  int periodsRemaining = n;
  
  for (int i = 0; i < cf->count; i++) {
    double amount = cf->flows[i].amount;
    int freq = cf->flows[i].frequency;

    for (int f = 0; f < freq; f++) {
      periodsRemaining--;
      /* Compound this cash flow for remaining periods */
      nfv += amount * pow(onePlusRate, (double)periodsRemaining);
    }
  }

  return nfv;
}

/* ============================================================
 * Payback Period (Pro only)
 * ============================================================ */

double cf_payback(CashFlowList *cf) {
  /*
   * Simple payback: Find when cumulative CF >= 0
   */

  double cumulative = cf->CF0;
  int period = 0;

  /* If CF0 is already positive, payback is immediate */
  if (cumulative >= 0) {
    return 0.0;
  }

  for (int i = 0; i < cf->count; i++) {
    double amount = cf->flows[i].amount;
    int freq = cf->flows[i].frequency;

    for (int f = 0; f < freq; f++) {
      period++;
      double prevCumulative = cumulative;
      cumulative += amount;

      if (cumulative >= 0) {
        /* Interpolate for fractional period */
        if (amount > 0) {
          double fraction = -prevCumulative / amount;
          return (double)(period - 1) + fraction;
        }
        return (double)period;
      }
    }
  }

  /* Never recovers */
  return -1.0;
}

/* ============================================================
 * Discounted Payback Period (Pro only) - OPTIMIZED
 * ============================================================ */

double cf_discounted_payback(CashFlowList *cf, double rate) {
  /*
   * Discounted payback: Find when cumulative discounted CF >= 0
   * 
   * OPTIMIZED: Use iterative discount factor instead of pow() each iteration
   */

  double cumulative = cf->CF0;
  int period = 0;
  double discountFactor = 1.0;
  double onePlusRate = 1.0 + rate;

  if (cumulative >= 0) {
    return 0.0;
  }

  for (int i = 0; i < cf->count; i++) {
    double amount = cf->flows[i].amount;
    int freq = cf->flows[i].frequency;

    for (int f = 0; f < freq; f++) {
      period++;
      discountFactor /= onePlusRate;  /* Replaces pow(1+r, period) */
      double discountedAmount = amount * discountFactor;
      double prevCumulative = cumulative;
      cumulative += discountedAmount;

      if (cumulative >= 0) {
        if (discountedAmount > 0) {
          double fraction = -prevCumulative / discountedAmount;
          return (double)(period - 1) + fraction;
        }
        return (double)period;
      }
    }
  }

  return -1.0;
}

/* ============================================================
 * Modified IRR (Pro only)
 * ============================================================ */

double cf_mirr(CashFlowList *cf, double financeRate, double reinvestRate,
               int *errorCode) {
  /*
   * MIRR formula:
   * MIRR = (FV of positive CFs at reinvest rate / PV of negative CFs at finance
   * rate)^(1/n) - 1
   */

  *errorCode = ERR_NONE;

  int n = cf_total_periods(cf);
  if (n == 0) {
    *errorCode = ERR_INVALID_INPUT;
    return 0.0;
  }

  double pvNegative = 0.0;
  double fvPositive = 0.0;
  int period = 0;

  /* Handle CF0 */
  if (cf->CF0 < 0) {
    pvNegative += -cf->CF0; /* Make positive for calculation */
  } else {
    fvPositive += cf->CF0 * pow(1.0 + reinvestRate, (double)n);
  }

  /* Process each cash flow */
  for (int i = 0; i < cf->count; i++) {
    double amount = cf->flows[i].amount;
    int freq = cf->flows[i].frequency;

    for (int f = 0; f < freq; f++) {
      period++;

      if (amount < 0) {
        pvNegative += -amount / pow(1.0 + financeRate, (double)period);
      } else {
        fvPositive += amount * pow(1.0 + reinvestRate, (double)(n - period));
      }
    }
  }

  if (pvNegative == 0.0) {
    *errorCode = ERR_NO_SOLUTION;
    return 0.0;
  }

  return pow(fvPositive / pvNegative, 1.0 / (double)n) - 1.0;
}
