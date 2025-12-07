/**
 * Open fx-BA: TI BA II Plus Clone
 * bond.c - Bond calculations implementation
 *
 * Implements:
 * - Bond price from yield
 * - Yield to maturity from price (Newton-Raphson)
 * - Accrued interest
 * - Macaulay duration
 * - Modified duration (Pro only)
 */

#include "bond.h"
#include "config.h"
#include <math.h>

/* ============================================================
 * Date Helper Functions
 * ============================================================ */

/* Reference date: January 1, 1900 */
#define REFERENCE_YEAR 1900

static int is_leap_year(int year) {
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static int days_in_month(int month, int year) {
  static const int days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  if (month == 2 && is_leap_year(year)) {
    return 29;
  }
  return days[month];
}

int date_to_days(int yyyymmdd) {
  int year = yyyymmdd / 10000;
  int month = (yyyymmdd / 100) % 100;
  int day = yyyymmdd % 100;

  int days = 0;

  /* Add days for complete years */
  for (int y = REFERENCE_YEAR; y < year; y++) {
    days += is_leap_year(y) ? 366 : 365;
  }

  /* Add days for complete months */
  for (int m = 1; m < month; m++) {
    days += days_in_month(m, year);
  }

  /* Add remaining days */
  days += day;

  return days;
}

int days_between(int date1, int date2, DayCountConvention convention) {
  int d1 = date1, d2 = date2;

  if (convention == DAY_COUNT_30_360) {
    /* 30/360 convention */
    int y1 = d1 / 10000, m1 = (d1 / 100) % 100, day1 = d1 % 100;
    int y2 = d2 / 10000, m2 = (d2 / 100) % 100, day2 = d2 % 100;

    /* Adjust day 31 to 30 */
    if (day1 == 31)
      day1 = 30;
    if (day2 == 31 && day1 >= 30)
      day2 = 30;

    return 360 * (y2 - y1) + 30 * (m2 - m1) + (day2 - day1);
  }

  /* Actual/Actual and other conventions */
  return date_to_days(date2) - date_to_days(date1);
}

int days_in_year(DayCountConvention convention) {
  switch (convention) {
  case DAY_COUNT_30_360:
  case DAY_COUNT_ACT_360:
    return 360;
  case DAY_COUNT_ACT_365:
    return 365;
  case DAY_COUNT_ACT_ACT:
  default:
    return 365; /* Simplified */
  }
}

/* ============================================================
 * Coupon Period Calculations
 * ============================================================ */

/* Calculate number of whole coupon periods remaining */
static double coupon_periods_remaining(BondInput *input) {
  int settleDays = date_to_days(input->settlementDate);
  int matureDays = date_to_days(input->maturityDate);

  int daysRemaining = matureDays - settleDays;
  int daysPerPeriod = days_in_year(input->dayCount) / (int)input->frequency;

  return (double)daysRemaining / (double)daysPerPeriod;
}

/* Calculate fraction of current period elapsed (for accrued interest) */
static double period_fraction_elapsed(BondInput *input) {
  /* Simplified: assume we're at the start of a period */
  /* In a full implementation, this would calculate based on last coupon date */
  double periods = coupon_periods_remaining(input);
  return periods - floor(periods);
}

/* ============================================================
 * Bond Price Calculation
 * ============================================================ */

double bond_price(BondInput *input, double yield) {
  /*
   * Bond price formula:
   * P = C * [1 - (1+r)^(-n)] / r + R * (1+r)^(-n)
   *
   * where:
   *   C = coupon payment per period
   *   r = yield per period
   *   n = number of periods
   *   R = redemption value
   */

  double couponPerPeriod = input->couponRate / (double)input->frequency;
  double yieldPerPeriod = yield / 100.0 / (double)input->frequency;
  double n = coupon_periods_remaining(input);
  double redemption = input->redemption;

  if (yieldPerPeriod == 0.0) {
    /* No discounting */
    return couponPerPeriod * n + redemption;
  }

  double discountFactor = pow(1.0 + yieldPerPeriod, -n);
  double annuityFactor = (1.0 - discountFactor) / yieldPerPeriod;

  double price = couponPerPeriod * annuityFactor + redemption * discountFactor;

  return price;
}

/* ============================================================
 * Yield to Maturity Calculation (Newton-Raphson)
 * ============================================================ */

static double bond_price_derivative(BondInput *input, double yield) {
  double h = 1e-6;
  double p1 = bond_price(input, yield + h);
  double p0 = bond_price(input, yield - h);
  return (p1 - p0) / (2.0 * h);
}

double bond_yield(BondInput *input, double price, int *errorCode) {
  *errorCode = 0;

  /* Initial guess based on coupon rate */
  double yield = input->couponRate;
  if (yield <= 0)
    yield = 5.0; /* Default 5% */

  /* Newton-Raphson iteration */
  for (int iter = 0; iter < MAX_ITERATIONS; iter++) {
    double calcPrice = bond_price(input, yield);
    double diff = calcPrice - price;

    if (fabs(diff) < TOLERANCE) {
      return yield;
    }

    double derivative = bond_price_derivative(input, yield);

    if (fabs(derivative) < 1e-10) {
      break;
    }

    double newYield = yield - diff / derivative;

    /* Bound the yield */
    if (newYield < 0.0)
      newYield = 0.001;
    if (newYield > 100.0)
      newYield = 100.0;

    if (fabs(newYield - yield) < TOLERANCE) {
      return newYield;
    }

    yield = newYield;
  }

  *errorCode = 3; /* ERR_ITERATION */
  return 0.0;
}

/* ============================================================
 * Accrued Interest
 * ============================================================ */

double bond_accrued_interest(BondInput *input) {
  /*
   * Accrued Interest = (Coupon Rate / Frequency) * (Days since last coupon /
   * Days in period)
   */

  double couponPerPeriod = input->couponRate / (double)input->frequency;
  double fraction = period_fraction_elapsed(input);

  return couponPerPeriod * fraction;
}

/* ============================================================
 * Macaulay Duration
 * ============================================================ */

double bond_duration(BondInput *input, double yield) {
  /*
   * Macaulay Duration:
   * D = Σ [t * CF(t) * (1+r)^(-t)] / Price
   */

  double couponPerPeriod = input->couponRate / (double)input->frequency;
  double yieldPerPeriod = yield / 100.0 / (double)input->frequency;
  double n = coupon_periods_remaining(input);
  int periods = (int)ceil(n);
  double redemption = input->redemption;

  double price = bond_price(input, yield);
  if (price <= 0)
    return 0.0;

  double weightedSum = 0.0;

  for (int t = 1; t <= periods; t++) {
    double cf = couponPerPeriod;
    if (t == periods) {
      cf += redemption; /* Add principal at maturity */
    }

    double pv = cf / pow(1.0 + yieldPerPeriod, (double)t);
    weightedSum += (double)t * pv;
  }

  /* Convert to years */
  double durationPeriods = weightedSum / price;
  double durationYears = durationPeriods / (double)input->frequency;

  return durationYears;
}

/* ============================================================
 * Modified Duration (Pro only)
 * ============================================================ */

double bond_modified_duration(BondInput *input, double yield) {
  /*
   * Modified Duration = Macaulay Duration / (1 + yield/frequency)
   */

  double macDuration = bond_duration(input, yield);
  double yieldPerPeriod = yield / 100.0 / (double)input->frequency;

  return macDuration / (1.0 + yieldPerPeriod);
}

/* ============================================================
 * Full Bond Calculation
 * ============================================================ */

BondResult bond_calculate(BondInput *input, double knownPrice,
                          double knownYield) {
  BondResult result = {0};
  int errorCode = 0;

  if (knownPrice > 0) {
    /* Solve for yield */
    result.price = knownPrice;
    result.yield = bond_yield(input, knownPrice, &errorCode);
  } else {
    /* Solve for price */
    result.yield = knownYield;
    result.price = bond_price(input, knownYield);
  }

  result.accruedInterest = bond_accrued_interest(input);
  result.dirtyPrice = result.price + result.accruedInterest;
  result.duration = bond_duration(input, result.yield);
  result.modDuration = bond_modified_duration(input, result.yield);

  return result;
}
