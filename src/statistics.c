/**
 * Open fx-BA: TI BA II Plus Clone
 * statistics.c - Statistics and Regression implementation
 *
 * Implements:
 * - 1-variable statistics (n, Σx, Σx², mean, Sx, σx)
 * - 2-variable statistics (Σy, Σy², Σxy, correlation)
 * - 4 regression types: Linear, Logarithmic, Exponential, Power
 */

#include "statistics.h"
#include <math.h>
#include <string.h>

/* ============================================================
 * Regression Type Names
 * ============================================================ */
static const char *REG_NAMES[] = {
    "LIN", /* Linear */
    "LOG", /* Logarithmic */
    "EXP", /* Exponential */
    "PWR"  /* Power */
};

const char *stat_regression_name(RegressionType type) {
  if (type > REG_POWER)
    return "???";
  return REG_NAMES[type];
}

/* ============================================================
 * Initialization
 * ============================================================ */

void stat_init(StatData *stat) {
  stat_clear(stat);
  stat->regType = REG_LINEAR;
}

void stat_clear(StatData *stat) {
  memset(stat->xData, 0, sizeof(stat->xData));
  memset(stat->yData, 0, sizeof(stat->yData));
  stat->count = 0;
}

/* ============================================================
 * Data Management
 * ============================================================ */

int stat_add_x(StatData *stat, double x) {
  if (stat->count >= STAT_MAX_POINTS)
    return 0;

  stat->xData[stat->count] = x;
  stat->yData[stat->count] = 0.0; /* Not used for 1-var */
  stat->count++;

  return 1;
}

int stat_add_xy(StatData *stat, double x, double y) {
  if (stat->count >= STAT_MAX_POINTS)
    return 0;

  stat->xData[stat->count] = x;
  stat->yData[stat->count] = y;
  stat->count++;

  return 1;
}

void stat_remove_last(StatData *stat) {
  if (stat->count > 0) {
    stat->count--;
  }
}

/* ============================================================
 * 1-Variable Statistics
 * ============================================================ */

Stat1VarResult stat_calc_1var(StatData *stat) {
  Stat1VarResult result = {0};

  if (stat->count == 0)
    return result;

  result.n = stat->count;

  /* First pass: sum, sum of squares, min, max */
  result.min = stat->xData[0];
  result.max = stat->xData[0];

  for (int i = 0; i < stat->count; i++) {
    double x = stat->xData[i];
    result.sum += x;
    result.sumSq += x * x;

    if (x < result.min)
      result.min = x;
    if (x > result.max)
      result.max = x;
  }

  /* Mean */
  result.mean = result.sum / (double)result.n;

  /* Standard deviation */
  if (result.n > 0) {
    /* Population std dev: sqrt(Σ(x-mean)² / n) */
    double variance =
        (result.sumSq / (double)result.n) - (result.mean * result.mean);
    if (variance < 0)
      variance = 0; /* Numerical stability */
    result.stdDevP = sqrt(variance);

    /* Sample std dev: sqrt(Σ(x-mean)² / (n-1)) */
    if (result.n > 1) {
      double sampleVar = (result.sumSq - result.n * result.mean * result.mean) /
                         (double)(result.n - 1);
      if (sampleVar < 0)
        sampleVar = 0;
      result.stdDevS = sqrt(sampleVar);
    }
  }

  return result;
}

/* ============================================================
 * 2-Variable Statistics
 * ============================================================ */

Stat2VarResult stat_calc_2var(StatData *stat) {
  Stat2VarResult result = {0};

  if (stat->count == 0)
    return result;

  result.n = stat->count;

  /* Calculate sums */
  for (int i = 0; i < stat->count; i++) {
    double x = stat->xData[i];
    double y = stat->yData[i];

    result.sumX += x;
    result.sumY += y;
    result.sumXSq += x * x;
    result.sumYSq += y * y;
    result.sumXY += x * y;
  }

  /* Means */
  result.meanX = result.sumX / (double)result.n;
  result.meanY = result.sumY / (double)result.n;

  /* Standard deviations */
  if (result.n > 0) {
    /* Population */
    double varX =
        (result.sumXSq / (double)result.n) - (result.meanX * result.meanX);
    double varY =
        (result.sumYSq / (double)result.n) - (result.meanY * result.meanY);
    if (varX < 0)
      varX = 0;
    if (varY < 0)
      varY = 0;
    result.stdDevXP = sqrt(varX);
    result.stdDevYP = sqrt(varY);

    /* Sample */
    if (result.n > 1) {
      double sVarX = (result.sumXSq - result.n * result.meanX * result.meanX) /
                     (double)(result.n - 1);
      double sVarY = (result.sumYSq - result.n * result.meanY * result.meanY) /
                     (double)(result.n - 1);
      if (sVarX < 0)
        sVarX = 0;
      if (sVarY < 0)
        sVarY = 0;
      result.stdDevXS = sqrt(sVarX);
      result.stdDevYS = sqrt(sVarY);
    }
  }

  return result;
}

/* ============================================================
 * Regression Calculations
 * ============================================================ */

/* Helper for linear regression on transformed data */
static RegressionResult linear_regression(double *x, double *y, int n) {
  RegressionResult result = {REG_LINEAR, 0, 0, 0, 0};

  if (n < 2)
    return result;

  double sumX = 0, sumY = 0, sumXY = 0, sumXSq = 0, sumYSq = 0;

  for (int i = 0; i < n; i++) {
    sumX += x[i];
    sumY += y[i];
    sumXY += x[i] * y[i];
    sumXSq += x[i] * x[i];
    sumYSq += y[i] * y[i];
  }

  double meanX = sumX / n;
  double meanY = sumY / n;

  /* b = Σ(x-x̄)(y-ȳ) / Σ(x-x̄)² */
  double sxy = sumXY - n * meanX * meanY;
  double sxx = sumXSq - n * meanX * meanX;
  double syy = sumYSq - n * meanY * meanY;

  if (sxx == 0) {
    result.b = 0;
  } else {
    result.b = sxy / sxx;
  }

  /* a = ȳ - b*x̄ */
  result.a = meanY - result.b * meanX;

  /* Correlation coefficient r */
  if (sxx > 0 && syy > 0) {
    result.r = sxy / sqrt(sxx * syy);
  }

  result.rSq = result.r * result.r;

  return result;
}

RegressionResult stat_regression(StatData *stat, RegressionType type) {
  RegressionResult result = {type, 0, 0, 0, 0};

  if (stat->count < 2)
    return result;

  double xTrans[STAT_MAX_POINTS];
  double yTrans[STAT_MAX_POINTS];
  int validCount = 0;

  switch (type) {
  case REG_LINEAR:
    /* y = a + bx (no transformation) */
    result = linear_regression(stat->xData, stat->yData, stat->count);
    result.type = REG_LINEAR;
    break;

  case REG_LOGARITHMIC:
    /* y = a + b*ln(x) → transform x = ln(x) */
    for (int i = 0; i < stat->count; i++) {
      if (stat->xData[i] > 0) {
        xTrans[validCount] = log(stat->xData[i]);
        yTrans[validCount] = stat->yData[i];
        validCount++;
      }
    }
    if (validCount >= 2) {
      result = linear_regression(xTrans, yTrans, validCount);
    }
    result.type = REG_LOGARITHMIC;
    break;

  case REG_EXPONENTIAL:
    /* y = a*e^(bx) → ln(y) = ln(a) + bx */
    for (int i = 0; i < stat->count; i++) {
      if (stat->yData[i] > 0) {
        xTrans[validCount] = stat->xData[i];
        yTrans[validCount] = log(stat->yData[i]);
        validCount++;
      }
    }
    if (validCount >= 2) {
      result = linear_regression(xTrans, yTrans, validCount);
      result.a = exp(result.a); /* Convert back from ln(a) */
    }
    result.type = REG_EXPONENTIAL;
    break;

  case REG_POWER:
    /* y = a*x^b → ln(y) = ln(a) + b*ln(x) */
    for (int i = 0; i < stat->count; i++) {
      if (stat->xData[i] > 0 && stat->yData[i] > 0) {
        xTrans[validCount] = log(stat->xData[i]);
        yTrans[validCount] = log(stat->yData[i]);
        validCount++;
      }
    }
    if (validCount >= 2) {
      result = linear_regression(xTrans, yTrans, validCount);
      result.a = exp(result.a); /* Convert back from ln(a) */
    }
    result.type = REG_POWER;
    break;
  }

  return result;
}

/* ============================================================
 * Prediction Functions
 * ============================================================ */

double stat_predict_y(RegressionResult *reg, double x) {
  switch (reg->type) {
  case REG_LINEAR:
    return reg->a + reg->b * x;

  case REG_LOGARITHMIC:
    if (x > 0) {
      return reg->a + reg->b * log(x);
    }
    return 0;

  case REG_EXPONENTIAL:
    return reg->a * exp(reg->b * x);

  case REG_POWER:
    if (x > 0) {
      return reg->a * pow(x, reg->b);
    }
    return 0;
  }
  return 0;
}

double stat_predict_x(RegressionResult *reg, double y) {
  switch (reg->type) {
  case REG_LINEAR:
    if (reg->b != 0) {
      return (y - reg->a) / reg->b;
    }
    return 0;

  case REG_LOGARITHMIC:
    if (reg->b != 0) {
      return exp((y - reg->a) / reg->b);
    }
    return 0;

  case REG_EXPONENTIAL:
    if (reg->a != 0 && y / reg->a > 0 && reg->b != 0) {
      return log(y / reg->a) / reg->b;
    }
    return 0;

  case REG_POWER:
    if (reg->a != 0 && y / reg->a > 0 && reg->b != 0) {
      return pow(y / reg->a, 1.0 / reg->b);
    }
    return 0;
  }
  return 0;
}
