/**
 * Open fx-BA: TI BA II Plus Clone
 * depreciation.h - Depreciation calculations (6 methods)
 *
 * TI BA II Plus Professional supports:
 * - SL: Straight Line
 * - SYD: Sum-of-Years' Digits
 * - DB: Declining Balance
 * - DB-SL: Declining Balance with crossover to SL
 * - SLF: Straight Line French (Pro only)
 * - DBF: Declining Balance French (Pro only)
 */

#ifndef DEPRECIATION_H
#define DEPRECIATION_H

/* ============================================================
 * Depreciation Methods (matching TI BA II Plus Professional)
 * ============================================================ */
typedef enum {
  DEPR_SL,    /* Straight Line */
  DEPR_SYD,   /* Sum-of-Years' Digits */
  DEPR_DB,    /* Declining Balance */
  DEPR_DB_SL, /* Declining Balance with crossover to SL */
  DEPR_SLF,   /* Straight Line French (Pro only) */
  DEPR_DBF,   /* Declining Balance French (Pro only) */
  DEPR_COUNT  /* Number of methods */
} DepreciationMethod;

/* ============================================================
 * Depreciation Input Structure
 * ============================================================ */
typedef struct {
  double cost;    /* Original cost of asset */
  double salvage; /* Salvage value at end of life */
  double life;    /* Life in years */
  double dbRate;  /* Declining balance rate (%, e.g., 200 for DDB) */
  int startMonth; /* Starting month (1-12), for partial year */
  int startYear;  /* Starting year of depreciation */
} DepreciationInput;

/* ============================================================
 * Depreciation Result Structure
 * ============================================================ */
typedef struct {
  int year;              /* Year number */
  double depreciation;   /* Depreciation for this year */
  double bookValueStart; /* Book value at start of year */
  double bookValueEnd;   /* Book value at end of year */
  double accumDepr;      /* Accumulated depreciation through this year */
  double remainingDepr;  /* Remaining depreciable amount */
} DepreciationResult;

/* ============================================================
 * Core Depreciation Functions
 * ============================================================ */

/**
 * Calculate depreciation for a specific year.
 * Handles partial year for first and last year automatically.
 */
DepreciationResult depr_calculate(DepreciationInput *input,
                                  DepreciationMethod method, int year);

/**
 * Get the name of a depreciation method.
 */
const char *depr_method_name(DepreciationMethod method);

/* ============================================================
 * Individual Method Functions
 * ============================================================ */

/**
 * Straight Line
 * DEP = (Cost - Salvage) / Life
 */
double depr_straight_line(double cost, double salvage, double life);

/**
 * Sum-of-Years' Digits
 * DEP(year) = (Cost - Salvage) * (Life - year + 1) / SumOfYears
 */
double depr_syd(double cost, double salvage, double life, int year);

/**
 * Declining Balance
 * DEP(year) = BookValue(year-1) * (Rate / 100 / Life)
 */
double depr_db(double cost, double salvage, double life, double dbRate,
               int year);

/**
 * Declining Balance with crossover to Straight Line
 */
double depr_db_sl(double cost, double salvage, double life, double dbRate,
                  int year);

/**
 * Straight Line French (Pro only)
 * Depreciation starts from 1st day of month following acquisition.
 * First year prorated based on remaining months.
 */
double depr_slf(double cost, double salvage, double life, int startMonth,
                int year);

/**
 * Declining Balance French (Pro only)
 * Uses French DB coefficients based on asset life.
 */
double depr_dbf(double cost, double salvage, double life, int startMonth,
                int year);

/**
 * Calculate partial year factor.
 * @param startMonth Starting month (1-12)
 * @param year Which year (1 = first year)
 * @param totalYears Total life in years
 * @return Factor between 0 and 1
 */
double depr_partial_year_factor(int startMonth, int year, double totalYears);

#endif /* DEPRECIATION_H */
