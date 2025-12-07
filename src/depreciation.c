/**
 * Open fx-BA: TI BA II Plus Clone
 * depreciation.c - Depreciation calculations implementation
 *
 * Implements all 6 depreciation methods from TI BA II Plus Professional:
 * - SL: Straight Line
 * - SYD: Sum-of-Years' Digits
 * - DB: Declining Balance
 * - DB-SL: DB with SL crossover
 * - SLF: Straight Line French
 * - DBF: Declining Balance French
 */

#include "depreciation.h"
#include <math.h>

/* ============================================================
 * Method Names
 * ============================================================ */
static const char *DEPR_METHOD_NAMES[] = {
    "SL",    /* Straight Line */
    "SYD",   /* Sum-of-Years' Digits */
    "DB",    /* Declining Balance */
    "DB-SL", /* DB with SL crossover */
    "SLF",   /* Straight Line French */
    "DBF"    /* Declining Balance French */
};

const char *depr_method_name(DepreciationMethod method) {
  if (method >= DEPR_COUNT)
    return "???";
  return DEPR_METHOD_NAMES[method];
}

/* ============================================================
 * French DB Coefficients (based on asset life)
 * ============================================================ */
static double french_db_coefficient(double life) {
  /* French declining balance coefficients */
  if (life <= 4.0)
    return 1.25;
  if (life <= 6.0)
    return 1.75;
  return 2.25; /* life > 6 years */
}

/* ============================================================
 * Helper: Sum of years from 1 to n
 * ============================================================ */
static double sum_of_years(double n) { return n * (n + 1.0) / 2.0; }

/* ============================================================
 * Partial Year Factor
 * ============================================================ */
double depr_partial_year_factor(int startMonth, int year, double totalYears) {
  /*
   * For first year: months remaining in year / 12
   * For last year: months used in final year / 12
   * For middle years: 1.0
   */
  if (year == 1) {
    /* First year: partial based on start month */
    /* If asset acquired in month M, depreciate for (12 - M + 1) months */
    /* For French methods, use remaining months */
    int monthsRemaining = 12 - startMonth + 1;
    return (double)monthsRemaining / 12.0;
  }

  /* Check if this is the last year (may need partial) */
  int totalYearsInt = (int)ceil(totalYears);
  int effectiveYears = totalYearsInt;

  /* If there's a partial first year, we may have an extra partial last year */
  if (startMonth > 1 && year == effectiveYears + 1) {
    int monthsUsedFirstYear = 12 - startMonth + 1;
    int monthsRemainingLastYear = 12 - monthsUsedFirstYear;
    return (double)monthsRemainingLastYear / 12.0;
  }

  return 1.0; /* Full year */
}

/* ============================================================
 * Straight Line Depreciation
 * ============================================================ */
double depr_straight_line(double cost, double salvage, double life) {
  if (life <= 0)
    return 0.0;
  return (cost - salvage) / life;
}

/* ============================================================
 * Sum-of-Years' Digits Depreciation
 * ============================================================ */
double depr_syd(double cost, double salvage, double life, int year) {
  if (life <= 0 || year < 1 || year > (int)life)
    return 0.0;

  double depreciableBase = cost - salvage;
  double soy = sum_of_years(life);
  double remainingLife = life - (double)year + 1.0;

  return depreciableBase * remainingLife / soy;
}

/* ============================================================
 * Declining Balance Depreciation
 * ============================================================ */
double depr_db(double cost, double salvage, double life, double dbRate,
               int year) {
  if (life <= 0 || year < 1)
    return 0.0;

  /* DB rate as decimal (e.g., 200% -> 2.0) */
  double rate = dbRate / 100.0 / life;

  /* Calculate book value at start of this year */
  double bookValue = cost;
  for (int y = 1; y < year; y++) {
    double dep = bookValue * rate;
    bookValue -= dep;

    /* Don't go below salvage */
    if (bookValue < salvage) {
      bookValue = salvage;
      break;
    }
  }

  /* This year's depreciation */
  double depreciation = bookValue * rate;

  /* Don't depreciate below salvage */
  if (bookValue - depreciation < salvage) {
    depreciation = bookValue - salvage;
  }

  if (depreciation < 0)
    depreciation = 0;

  return depreciation;
}

/* ============================================================
 * Declining Balance with SL Crossover
 * ============================================================ */
double depr_db_sl(double cost, double salvage, double life, double dbRate,
                  int year) {
  if (life <= 0 || year < 1)
    return 0.0;

  double rate = dbRate / 100.0 / life;
  double bookValue = cost;
  double depreciation = 0.0;

  for (int y = 1; y <= year; y++) {
    double remainingLife = life - (double)y + 1.0;

    /* Calculate DB depreciation */
    double dbDep = bookValue * rate;

    /* Calculate what SL would give for remaining life */
    double slDep = (bookValue - salvage) / remainingLife;

    /* Use whichever is higher (crossover point) */
    depreciation = (slDep > dbDep) ? slDep : dbDep;

    /* Don't depreciate below salvage */
    if (bookValue - depreciation < salvage) {
      depreciation = bookValue - salvage;
    }

    if (depreciation < 0)
      depreciation = 0;

    if (y < year) {
      bookValue -= depreciation;
    }
  }

  return depreciation;
}

/* ============================================================
 * Straight Line French (Pro only)
 * ============================================================ */
double depr_slf(double cost, double salvage, double life, int startMonth,
                int year) {
  if (life <= 0 || year < 1)
    return 0.0;

  /* Annual SL depreciation */
  double annualDepr = (cost - salvage) / life;

  /* Apply partial year factor */
  double factor = depr_partial_year_factor(startMonth, year, life);

  return annualDepr * factor;
}

/* ============================================================
 * Declining Balance French (Pro only)
 * ============================================================ */
double depr_dbf(double cost, double salvage, double life, int startMonth,
                int year) {
  if (life <= 0 || year < 1)
    return 0.0;

  /* Get French coefficient based on asset life */
  double coef = french_db_coefficient(life);
  double rate = coef / life;

  /* Calculate depreciation with partial year handling */
  double bookValue = cost;
  double depreciation = 0.0;

  for (int y = 1; y <= year; y++) {
    double factor = depr_partial_year_factor(startMonth, y, life);

    /* DB depreciation for this year */
    double dbDep = bookValue * rate * factor;

    /* Calculate remaining life for SL crossover check */
    double remainingLife = life - (double)y + 1.0;
    double slDep = (bookValue - salvage) / remainingLife * factor;

    /* French method: switch to SL when SL > DB */
    depreciation = (slDep > dbDep) ? slDep : dbDep;

    /* Don't depreciate below salvage */
    if (bookValue - depreciation < salvage) {
      depreciation = bookValue - salvage;
    }

    if (depreciation < 0)
      depreciation = 0;

    if (y < year) {
      bookValue -= depreciation;
    }
  }

  return depreciation;
}

/* ============================================================
 * Main Depreciation Calculator
 * ============================================================ */
DepreciationResult depr_calculate(DepreciationInput *input,
                                  DepreciationMethod method, int year) {
  DepreciationResult result = {0};

  if (!input || year < 1)
    return result;

  double cost = input->cost;
  double salvage = input->salvage;
  double life = input->life;
  double dbRate = input->dbRate;
  int startMonth = input->startMonth;

  if (startMonth < 1)
    startMonth = 1;
  if (startMonth > 12)
    startMonth = 12;

  result.year = year;
  result.bookValueStart = cost;

  /* Calculate depreciation based on method */
  switch (method) {
  case DEPR_SL:
    result.depreciation = depr_straight_line(cost, salvage, life);
    break;
  case DEPR_SYD:
    result.depreciation = depr_syd(cost, salvage, life, year);
    break;
  case DEPR_DB:
    result.depreciation = depr_db(cost, salvage, life, dbRate, year);
    break;
  case DEPR_DB_SL:
    result.depreciation = depr_db_sl(cost, salvage, life, dbRate, year);
    break;
  case DEPR_SLF:
    result.depreciation = depr_slf(cost, salvage, life, startMonth, year);
    break;
  case DEPR_DBF:
    result.depreciation = depr_dbf(cost, salvage, life, startMonth, year);
    break;
  default:
    result.depreciation = 0.0;
    break;
  }

  /* Calculate accumulated depreciation and book value */
  result.accumDepr = 0.0;
  double bookValue = cost;

  for (int y = 1; y <= year; y++) {
    double yearDep;

    switch (method) {
    case DEPR_SL:
      yearDep = depr_straight_line(cost, salvage, life);
      break;
    case DEPR_SYD:
      yearDep = depr_syd(cost, salvage, life, y);
      break;
    case DEPR_DB:
      yearDep = depr_db(cost, salvage, life, dbRate, y);
      break;
    case DEPR_DB_SL:
      yearDep = depr_db_sl(cost, salvage, life, dbRate, y);
      break;
    case DEPR_SLF:
      yearDep = depr_slf(cost, salvage, life, startMonth, y);
      break;
    case DEPR_DBF:
      yearDep = depr_dbf(cost, salvage, life, startMonth, y);
      break;
    default:
      yearDep = 0.0;
      break;
    }

    if (y == year) {
      result.bookValueStart = bookValue;
    }

    result.accumDepr += yearDep;
    bookValue -= yearDep;
  }

  result.bookValueEnd = bookValue;

  /* Ensure book value doesn't go below salvage */
  if (result.bookValueEnd < salvage) {
    result.bookValueEnd = salvage;
  }

  /* Remaining depreciable amount */
  result.remainingDepr = result.bookValueEnd - salvage;
  if (result.remainingDepr < 0)
    result.remainingDepr = 0;

  return result;
}
