/**
 * Open fx-BA: TI BA II Plus Clone
 * statistics.h - 1-var/2-var statistics with 4 regression types
 */

#ifndef STATISTICS_H
#define STATISTICS_H

/* ============================================================
 * Maximum Data Points
 * ============================================================ */
#define STAT_MAX_POINTS 50

/* ============================================================
 * Regression Types
 * ============================================================ */
typedef enum {
  REG_LINEAR,      /* y = a + bx */
  REG_LOGARITHMIC, /* y = a + b*ln(x) */
  REG_EXPONENTIAL, /* y = a * e^(bx) */
  REG_POWER        /* y = a * x^b */
} RegressionType;

/* ============================================================
 * 1-Variable Statistics Results
 * ============================================================ */
typedef struct {
  int n;          /* Number of data points */
  double sum;     /* Σx */
  double sumSq;   /* Σx² */
  double mean;    /* x̄ (mean) */
  double stdDevS; /* Sx (sample std deviation) */
  double stdDevP; /* σx (population std deviation) */
  double min;     /* Minimum value */
  double max;     /* Maximum value */
} Stat1VarResult;

/* ============================================================
 * 2-Variable Statistics Results
 * ============================================================ */
typedef struct {
  int n;           /* Number of data points */
  double sumX;     /* Σx */
  double sumY;     /* Σy */
  double sumXSq;   /* Σx² */
  double sumYSq;   /* Σy² */
  double sumXY;    /* Σxy */
  double meanX;    /* x̄ */
  double meanY;    /* ȳ */
  double stdDevXS; /* Sx */
  double stdDevYS; /* Sy */
  double stdDevXP; /* σx */
  double stdDevYP; /* σy */
} Stat2VarResult;

/* ============================================================
 * Regression Results
 * ============================================================ */
typedef struct {
  RegressionType type;
  double a;   /* Intercept (or coefficient) */
  double b;   /* Slope (or exponent) */
  double r;   /* Correlation coefficient */
  double rSq; /* R² (coefficient of determination) */
} RegressionResult;

/* ============================================================
 * Statistics Data Storage
 * ============================================================ */
typedef struct {
  double xData[STAT_MAX_POINTS];
  double yData[STAT_MAX_POINTS];
  int count;
  RegressionType regType;
} StatData;

/* ============================================================
 * Statistics Functions
 * ============================================================ */

/**
 * Initialize statistics data.
 */
void stat_init(StatData *stat);

/**
 * Clear all data.
 */
void stat_clear(StatData *stat);

/**
 * Add a data point (1-variable).
 */
int stat_add_x(StatData *stat, double x);

/**
 * Add a data point (2-variable).
 */
int stat_add_xy(StatData *stat, double x, double y);

/**
 * Remove last data point.
 */
void stat_remove_last(StatData *stat);

/**
 * Calculate 1-variable statistics.
 */
Stat1VarResult stat_calc_1var(StatData *stat);

/**
 * Calculate 2-variable statistics.
 */
Stat2VarResult stat_calc_2var(StatData *stat);

/**
 * Calculate regression.
 */
RegressionResult stat_regression(StatData *stat, RegressionType type);

/**
 * Predict y from x using current regression.
 */
double stat_predict_y(RegressionResult *reg, double x);

/**
 * Predict x from y using current regression.
 */
double stat_predict_x(RegressionResult *reg, double y);

/**
 * Get regression type name.
 */
const char *stat_regression_name(RegressionType type);

#endif /* STATISTICS_H */
