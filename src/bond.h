/**
 * Open fx-BA: TI BA II Plus Clone
 * bond.h - Bond calculations (Price, Yield, Duration)
 */

#ifndef BOND_H
#define BOND_H

/* ============================================================
 * Day Count Conventions
 * ============================================================ */
typedef enum {
  DAY_COUNT_ACT_ACT, /* Actual/Actual (ISDA) */
  DAY_COUNT_30_360,  /* 30/360 (US) */
  DAY_COUNT_ACT_360, /* Actual/360 */
  DAY_COUNT_ACT_365  /* Actual/365 */
} DayCountConvention;

/* ============================================================
 * Coupon Frequency
 * ============================================================ */
typedef enum {
  COUPON_ANNUAL = 1,
  COUPON_SEMI_ANNUAL = 2,
  COUPON_QUARTERLY = 4,
  COUPON_MONTHLY = 12
} CouponFrequency;

/* ============================================================
 * Bond Input Structure
 * ============================================================ */
typedef struct {
  /* Settlement date (as days from a reference, or YYYYMMDD) */
  int settlementDate;

  /* Maturity date */
  int maturityDate;

  /* Annual coupon rate (%) */
  double couponRate;

  /* Redemption value (% of par, usually 100) */
  double redemption;

  /* Coupon frequency */
  CouponFrequency frequency;

  /* Day count convention */
  DayCountConvention dayCount;
} BondInput;

/* ============================================================
 * Bond Result Structure
 * ============================================================ */
typedef struct {
  double price;           /* Clean price (% of par) */
  double yield;           /* Yield to maturity (%) */
  double accruedInterest; /* Accrued interest (% of par) */
  double dirtyPrice;      /* Clean price + accrued interest */
  double duration;        /* Macaulay duration (years) */
  double modDuration;     /* Modified duration (Pro only) */
} BondResult;

/* ============================================================
 * Bond Calculation Functions
 * ============================================================ */

/**
 * Calculate bond price given yield.
 *
 * @param input Bond parameters
 * @param yield Yield to maturity (% annual)
 * @return Price as % of par
 */
double bond_price(BondInput *input, double yield);

/**
 * Calculate yield to maturity given price.
 * Uses Newton-Raphson iteration.
 *
 * @param input Bond parameters
 * @param price Clean price (% of par)
 * @param errorCode Output: set if no solution found
 * @return Yield to maturity (%)
 */
double bond_yield(BondInput *input, double price, int *errorCode);

/**
 * Calculate accrued interest.
 *
 * @param input Bond parameters
 * @return Accrued interest (% of par)
 */
double bond_accrued_interest(BondInput *input);

/**
 * Calculate Macaulay duration.
 *
 * @param input Bond parameters
 * @param yield Yield to maturity (%)
 * @return Duration in years
 */
double bond_duration(BondInput *input, double yield);

/**
 * Calculate Modified duration (Pro only).
 * ModDur = MacDur / (1 + yield/frequency)
 *
 * @param input Bond parameters
 * @param yield Yield to maturity (%)
 * @return Modified duration
 */
double bond_modified_duration(BondInput *input, double yield);

/**
 * Full bond calculation - computes all values.
 *
 * @param input Bond parameters
 * @param knownPrice If > 0, solve for yield; otherwise solve for price using
 * knownYield
 * @param knownYield Used if knownPrice <= 0
 * @return Complete bond result
 */
BondResult bond_calculate(BondInput *input, double knownPrice,
                          double knownYield);

/* ============================================================
 * Date Helper Functions
 * ============================================================ */

/**
 * Convert YYYYMMDD to days since reference date.
 */
int date_to_days(int yyyymmdd);

/**
 * Calculate days between two dates using specified convention.
 */
int days_between(int date1, int date2, DayCountConvention convention);

/**
 * Get the number of days in a year for the convention.
 */
int days_in_year(DayCountConvention convention);

#endif /* BOND_H */
