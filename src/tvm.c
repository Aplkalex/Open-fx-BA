/**
 * Open fx-BA: TI BA II Plus Clone
 * tvm.c - Time Value of Money calculations implementation
 *
 * Formulas based on TI BA II Plus calculation methodology.
 * Uses standard annuity formulas with BEGIN/END mode support.
 */

#include "tvm.h"
#include "config.h"
#include <math.h>

/* ============================================================
 * Helper Functions
 * ============================================================ */

double tvm_periodic_rate(double annualRate, double py, double cy) {
  /*
   * Convert annual I/Y to periodic rate.
   *
   * If P/Y == C/Y:
   *   periodic_rate = I/Y / P/Y / 100
   *
   * If P/Y != C/Y (rate conversion needed):
   *   periodic_rate = ((1 + I/Y/(100*C/Y))^(C/Y/P/Y)) - 1
   */

  if (annualRate == 0.0)
    return 0.0;

  double rate;

  if (py == cy) {
    rate = annualRate / (100.0 * py);
  } else {
    /* Rate conversion */
    double nominalRate = annualRate / 100.0;
    rate = pow(1.0 + nominalRate / cy, cy / py) - 1.0;
  }

  return rate;
}

/* ============================================================
 * TVM Solver - Main Entry Point
 * ============================================================ */

double tvm_solve_for(Calculator *calc, TVMVariable solveFor) {
  TVM_Data *tvm = &calc->tvm;

  /* Get periodic interest rate */
  double rate = tvm_periodic_rate(tvm->I_Y, tvm->P_Y, tvm->C_Y);

  double result = 0.0;
  calc->errorCode = ERR_NONE;

  switch (solveFor) {
  case TVM_VAR_N:
    result = tvm_calc_n(rate, tvm->PV, tvm->PMT, tvm->FV, tvm->mode);
    tvm->N = result;
    break;

  case TVM_VAR_IY:
    result = tvm_calc_iy(tvm->N, tvm->PV, tvm->PMT, tvm->FV, tvm->mode,
                         &calc->errorCode);
    if (calc->errorCode == ERR_NONE) {
      /* Convert back to annual % */
      result = result * 100.0 * tvm->P_Y;
      tvm->I_Y = result;
    }
    break;

  case TVM_VAR_PV:
    result = tvm_calc_pv(tvm->N, rate, tvm->PMT, tvm->FV, tvm->mode);
    tvm->PV = result;
    break;

  case TVM_VAR_PMT:
    result = tvm_calc_pmt(tvm->N, rate, tvm->PV, tvm->FV, tvm->mode);
    tvm->PMT = result;
    break;

  case TVM_VAR_FV:
    result = tvm_calc_fv(tvm->N, rate, tvm->PV, tvm->PMT, tvm->mode);
    tvm->FV = result;
    break;

  default:
    calc->errorCode = ERR_INVALID_INPUT;
    break;
  }

  return result;
}

/* ============================================================
 * Individual TVM Functions
 * ============================================================ */

double tvm_calc_fv(double n, double rate, double pv, double pmt, TVMMode mode) {
  /*
   * FV = -( PV * (1+i)^n + PMT * [(1+i)^n - 1] / i * (1+i*k) )
   * where k = 1 for BEGIN mode, k = 0 for END mode
   * 
   * OPTIMIZED: Reuse compoundFactor, avoid redundant calculations
   */

  if (rate == 0.0) {
    /* Simple case: no interest */
    return -(pv + pmt * n);
  }

  double compoundFactor = pow(1.0 + rate, n);
  double annuityFactor = (compoundFactor - 1.0) / rate;
  
  /* BGN mode multiplier: (1+i) for BEGIN, 1 for END */
  double modeMultiplier = (mode == TVM_BEGIN) ? (1.0 + rate) : 1.0;

  return -(pv * compoundFactor + pmt * annuityFactor * modeMultiplier);
}

double tvm_calc_pv(double n, double rate, double pmt, double fv, TVMMode mode) {
  /*
   * PV = -( FV / (1+i)^n + PMT * [1 - (1+i)^(-n)] / i * (1+i*k) )
   * 
   * OPTIMIZED: Single pow() call, reuse discountFactor
   */

  if (rate == 0.0) {
    return -(fv + pmt * n);
  }

  double discountFactor = pow(1.0 + rate, -n);
  double annuityFactor = (1.0 - discountFactor) / rate;
  double modeMultiplier = (mode == TVM_BEGIN) ? (1.0 + rate) : 1.0;

  return -(fv * discountFactor + pmt * annuityFactor * modeMultiplier);
}

double tvm_calc_pmt(double n, double rate, double pv, double fv, TVMMode mode) {
  /*
   * PMT = -(PV + FV/(1+i)^n) / ( [1-(1+i)^(-n)]/i * (1+i*k) )
   * 
   * OPTIMIZED: Single pow() call
   */

  if (rate == 0.0) {
    if (n == 0.0)
      return 0.0;
    return -(pv + fv) / n;
  }

  double discountFactor = pow(1.0 + rate, -n);
  double annuityFactor = (1.0 - discountFactor) / rate;
  double modeMultiplier = (mode == TVM_BEGIN) ? (1.0 + rate) : 1.0;

  return -(pv + fv * discountFactor) / (annuityFactor * modeMultiplier);
}

double tvm_calc_n(double rate, double pv, double pmt, double fv, TVMMode mode) {
  /*
   * N = ln[(PMT*(1+i*k) - FV*i) / (PMT*(1+i*k) + PV*i)] / ln(1+i)
   * 
   * OPTIMIZED: Use log1p for better precision when rate is small
   */

  if (rate == 0.0) {
    if (pmt == 0.0)
      return 0.0;
    return -(pv + fv) / pmt;
  }

  double modeMultiplier = (mode == TVM_BEGIN) ? (1.0 + rate) : 1.0;
  double pmtAdj = pmt * modeMultiplier;
  double numerator = pmtAdj - fv * rate;
  double denominator = pmtAdj + pv * rate;

  if (denominator == 0.0 || numerator / denominator <= 0.0) {
    return 0.0; /* Error case */
  }

  /* Use log1p(rate) instead of log(1+rate) for better precision */
  return log(numerator / denominator) / log1p(rate);
}

/* ============================================================
 * Newton-Raphson Method for I/Y (OPTIMIZED)
 * ============================================================ */

/*
 * The TVM equation: PV + PMT * A + FV * D = 0
 * where:
 *   D = (1+i)^(-n)           (discount factor)
 *   A = (1-D)/i * M          (annuity factor, M = mode multiplier)
 *
 * OPTIMIZATION: Compute function and derivative together to reuse
 * intermediate values and avoid redundant pow() calls.
 */

static void tvm_function_and_derivative(double rate, double n, double pv,
                                        double pmt, double fv, double modeMultiplier,
                                        double *f, double *df) {
  if (rate == 0.0) {
    *f = pv + pmt * n + fv;
    *df = 0.0;
    return;
  }

  /* Compute powers once */
  double onePlusRate = 1.0 + rate;
  double compFactor = pow(onePlusRate, n);     /* (1+i)^n */
  double discFactor = 1.0 / compFactor;         /* (1+i)^(-n) */
  
  /* Annuity factor: (1 - D) / i * M */
  double annuityFactor = (1.0 - discFactor) / rate * modeMultiplier;
  
  /* Function value: f(i) = PV + PMT*A + FV*D */
  *f = pv + pmt * annuityFactor + fv * discFactor;
  
  /*
   * ANALYTICAL DERIVATIVE (much faster than numerical):
   * 
   * d(discFactor)/di = -n * (1+i)^(-n-1) = -n * D / (1+i)
   * 
   * For annuity factor A = (1-D)/i * M:
   * dA/di = M * [(-dD/di * i - (1-D)) / i^2]
   *       = M * [(n*D/(1+i) * i - (1-D)) / i^2]
   *       = M * [(n*D*i/(1+i) - 1 + D) / i^2]
   * 
   * df/di = PMT * dA/di + FV * dD/di
   */
  double dDisc = -n * discFactor / onePlusRate;
  double dAnnuity = modeMultiplier * (n * discFactor * rate / onePlusRate - 1.0 + discFactor) / (rate * rate);
  
  *df = pmt * dAnnuity + fv * dDisc;
}

double tvm_calc_iy(double n, double pv, double pmt, double fv, TVMMode mode,
                   int *errorCode) {
  double modeMultiplier = (mode == TVM_BEGIN) ? (1.0 + 0.1) : 1.0; /* Will adjust */

  /* Special case: if all zeros */
  if (pv == 0.0 && pmt == 0.0 && fv == 0.0) {
    *errorCode = ERR_INVALID_INPUT;
    return 0.0;
  }

  /* Special case: no PMT, simple compound interest */
  if (pmt == 0.0 && pv != 0.0 && fv != 0.0 && n > 0.0) {
    double ratio = -fv / pv;
    if (ratio > 0.0) {
      *errorCode = ERR_NONE;
      return pow(ratio, 1.0 / n) - 1.0;
    }
  }

  /* Initial guess - use better heuristics */
  double rate = INITIAL_GUESS;

  /* Try to get a better initial guess based on sign of cash flows */
  if (pv != 0.0 && fv != 0.0) {
    double ratio = -fv / pv;
    if (ratio > 0.0 && n > 0.0) {
      rate = pow(ratio, 1.0 / n) - 1.0;
      if (rate <= 0.0 || rate > 1.0)
        rate = INITIAL_GUESS;
    }
  }

  /* Newton-Raphson iteration with optimized function evaluation */
  double f, df;
  
  for (int iter = 0; iter < MAX_ITERATIONS; iter++) {
    /* Update mode multiplier for current rate */
    modeMultiplier = (mode == TVM_BEGIN) ? (1.0 + rate) : 1.0;
    
    /* Compute function and derivative in one call (optimized) */
    tvm_function_and_derivative(rate, n, pv, pmt, fv, modeMultiplier, &f, &df);

    /* Check for convergence */
    if (fabs(f) < TOLERANCE) {
      *errorCode = ERR_NONE;
      return rate;
    }

    if (fabs(df) < 1e-15) {
      /* Derivative too small, stuck */
      break;
    }

    double newRate = rate - f / df;

    /* Bound the rate to reasonable values */
    if (newRate < -0.999)
      newRate = -0.999;
    if (newRate > 10.0)
      newRate = 10.0;  /* 1000% max */

    /* Check for convergence */
    if (fabs(newRate - rate) < TOLERANCE) {
      *errorCode = ERR_NONE;
      return newRate;
    }

    rate = newRate;
  }

  /* Failed to converge */
  *errorCode = ERR_ITERATION;
  return 0.0;
}

/* ============================================================
 * Amortization (OPTIMIZED with closed-form formulas)
 * ============================================================ */

/*
 * Balance at period p (closed-form):
 * B(p) = PV * (1+i)^p + PMT * [(1+i)^p - 1] / i
 * 
 * This avoids O(p) loop iterations.
 */
static double amort_balance_at(int period, double rate, double pv, double pmt) {
  if (rate == 0.0) {
    return pv + pmt * period;
  }
  double compFactor = pow(1.0 + rate, period);
  return pv * compFactor + pmt * (compFactor - 1.0) / rate;
}

AmortResult tvm_amort_period(int period, double n, double rate, double pv,
                             double pmt) {
  AmortResult result = {0.0, 0.0, 0.0};

  if (period < 1 || period > (int)n) {
    return result;
  }

  /* OPTIMIZED: Use closed-form balance formula instead of loop */
  double balanceStart = amort_balance_at(period - 1, rate, pv, pmt);
  
  /* This period's amortization */
  result.interest = balanceStart * rate;
  result.principal = pmt - result.interest;
  result.balance = balanceStart - result.principal;

  return result;
}

void tvm_amort_range(int startPeriod, int endPeriod, double n, double rate,
                     double pv, double pmt, double *totalPrincipal,
                     double *totalInterest, double *endBalance) {

  (void)n;  /* Parameter kept for API compatibility */
  
  /* OPTIMIZED: Use closed-form formulas */
  
  /* Balance at end of range */
  *endBalance = amort_balance_at(endPeriod, rate, pv, pmt);
  
  /* Balance at start of range (period before startPeriod) */
  double balanceAtStart = amort_balance_at(startPeriod - 1, rate, pv, pmt);
  
  /* Total principal = change in balance */
  *totalPrincipal = balanceAtStart - *endBalance;
  
  /* Total payments in range */
  int numPayments = endPeriod - startPeriod + 1;
  double totalPayments = pmt * numPayments;
  
  /* Total interest = total payments - total principal paid */
  *totalInterest = totalPayments - *totalPrincipal;
}
