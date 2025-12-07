/**
 * Open fx-BA: TI BA II Plus Clone
 * features.c - Pro/Standard mode feature flagging implementation
 */

#include "features.h"

/* ============================================================
 * Feature Database
 * ============================================================ */

static const FeatureInfo featureDatabase[] = {
    /* TVM Features - Available in both */
    {FEATURE_TVM_N, "N", "Number of periods", 1, 1},
    {FEATURE_TVM_IY, "I/Y", "Interest per year", 1, 1},
    {FEATURE_TVM_PV, "PV", "Present value", 1, 1},
    {FEATURE_TVM_PMT, "PMT", "Payment", 1, 1},
    {FEATURE_TVM_FV, "FV", "Future value", 1, 1},
    {FEATURE_AMORTIZATION, "AMORT", "Amortization schedule", 1, 1},

    /* Cash Flow Features */
    {FEATURE_CF_NPV, "NPV", "Net present value", 1, 1},
    {FEATURE_CF_IRR, "IRR", "Internal rate of return", 1, 1},
    {FEATURE_CF_NFV, "NFV", "Net future value", 0, 1},   /* Pro only */
    {FEATURE_CF_PB, "PB", "Payback period", 0, 1},       /* Pro only */
    {FEATURE_CF_DPB, "DPB", "Discounted payback", 0, 1}, /* Pro only */
    {FEATURE_CF_MIRR, "MIRR", "Modified IRR", 0, 1},     /* Pro only */

    /* Bond Features */
    {FEATURE_BOND_PRICE, "PRI", "Bond price", 1, 1},
    {FEATURE_BOND_YIELD, "YLD", "Yield to maturity", 1, 1},
    {FEATURE_BOND_AI, "AI", "Accrued interest", 1, 1},
    {FEATURE_BOND_DUR, "DUR", "Macaulay duration", 0, 1},     /* Pro only */
    {FEATURE_BOND_MOD_DUR, "MOD", "Modified duration", 0, 1}, /* Pro only */

    /* Depreciation Features */
    {FEATURE_DEPR_SL, "SL", "Straight line", 1, 1},
    {FEATURE_DEPR_SYD, "SYD", "Sum of years digits", 1, 1},
    {FEATURE_DEPR_DB, "DB", "Declining balance", 0, 1},         /* Pro only */
    {FEATURE_DEPR_DB_SL, "DBSL", "DB with SL crossover", 0, 1}, /* Pro only */

    /* Statistics Features */
    {FEATURE_STAT_1VAR, "1-VAR", "1-variable statistics", 1, 1},
    {FEATURE_STAT_2VAR, "2-VAR", "2-variable statistics", 1, 1},
    {FEATURE_STAT_LIN_REG, "LIN", "Linear regression", 1, 1},
    {FEATURE_STAT_FORECAST, "FCST", "Forecasting", 0, 1}, /* Pro only */

    /* Other Features */
    {FEATURE_DATE_CALC, "DATE", "Date calculations", 1, 1},
    {FEATURE_BREAKEVEN, "BRKEVN", "Breakeven analysis", 0, 1},   /* Pro only */
    {FEATURE_MEMORY_PLUS, "M0-M9", "10 memory registers", 0, 1}, /* Pro only */
};

#define FEATURE_DB_SIZE (sizeof(featureDatabase) / sizeof(featureDatabase[0]))

/* ============================================================
 * Feature Checking
 * ============================================================ */

int feature_is_available(CalculatorModel model, FeatureID feature) {
  if (feature >= FEATURE_COUNT)
    return 0;

  const FeatureInfo *info = &featureDatabase[feature];

  if (model == MODEL_PROFESSIONAL) {
    return info->professionalMode;
  } else {
    return info->standardMode;
  }
}

const FeatureInfo *feature_get_info(FeatureID feature) {
  if (feature >= FEATURE_COUNT)
    return (void *)0;
  return &featureDatabase[feature];
}

const char *feature_get_name(FeatureID feature) {
  if (feature >= FEATURE_COUNT)
    return "???";
  return featureDatabase[feature].name;
}

int feature_is_pro_only(FeatureID feature) {
  if (feature >= FEATURE_COUNT)
    return 0;

  const FeatureInfo *info = &featureDatabase[feature];
  return (info->professionalMode && !info->standardMode);
}

/* ============================================================
 * Mode Management
 * ============================================================ */

const char *model_get_name(CalculatorModel model) {
  switch (model) {
  case MODEL_PROFESSIONAL:
    return "TI BA II Plus Professional";
  case MODEL_STANDARD:
  default:
    return "TI BA II Plus";
  }
}

const char *model_get_indicator(CalculatorModel model) {
  switch (model) {
  case MODEL_PROFESSIONAL:
    return "PRO";
  case MODEL_STANDARD:
  default:
    return "STD";
  }
}

CalculatorModel model_toggle(CalculatorModel current) {
  if (current == MODEL_STANDARD) {
    return MODEL_PROFESSIONAL;
  } else {
    return MODEL_STANDARD;
  }
}

/* ============================================================
 * Feature Listing
 * ============================================================ */

int feature_list_pro_only(FeatureID *buffer, int maxCount) {
  int count = 0;

  for (int i = 0; i < (int)FEATURE_DB_SIZE && count < maxCount; i++) {
    if (feature_is_pro_only((FeatureID)i)) {
      buffer[count++] = (FeatureID)i;
    }
  }

  return count;
}

int feature_list_common(FeatureID *buffer, int maxCount) {
  int count = 0;

  for (int i = 0; i < (int)FEATURE_DB_SIZE && count < maxCount; i++) {
    const FeatureInfo *info = &featureDatabase[i];
    if (info->standardMode && info->professionalMode) {
      buffer[count++] = (FeatureID)i;
    }
  }

  return count;
}
