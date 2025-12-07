/**
 * Open fx-BA: TI BA II Plus Clone
 * features.h - Pro/Standard mode feature flagging
 *
 * Implements the feature differences between:
 * - TI BA II Plus (Standard)
 * - TI BA II Plus Professional
 */

#ifndef FEATURES_H
#define FEATURES_H

#include "types.h"

/* ============================================================
 * Feature IDs
 * ============================================================ */
typedef enum {
  /* TVM Features (both modes) */
  FEATURE_TVM_N,
  FEATURE_TVM_IY,
  FEATURE_TVM_PV,
  FEATURE_TVM_PMT,
  FEATURE_TVM_FV,
  FEATURE_AMORTIZATION,

  /* Cash Flow Features */
  FEATURE_CF_NPV,  /* Both */
  FEATURE_CF_IRR,  /* Both */
  FEATURE_CF_NFV,  /* Pro only */
  FEATURE_CF_PB,   /* Pro only - Payback */
  FEATURE_CF_DPB,  /* Pro only - Discounted Payback */
  FEATURE_CF_MIRR, /* Pro only - Modified IRR */

  /* Bond Features */
  FEATURE_BOND_PRICE,   /* Both */
  FEATURE_BOND_YIELD,   /* Both */
  FEATURE_BOND_AI,      /* Both - Accrued Interest */
  FEATURE_BOND_DUR,     /* Pro only - Duration */
  FEATURE_BOND_MOD_DUR, /* Pro only - Modified Duration */

  /* Depreciation Features */
  FEATURE_DEPR_SL,    /* Both - Straight Line */
  FEATURE_DEPR_SYD,   /* Both - Sum of Years Digits */
  FEATURE_DEPR_DB,    /* Pro only - Declining Balance */
  FEATURE_DEPR_DB_SL, /* Pro only - DB with SL crossover */

  /* Statistics Features */
  FEATURE_STAT_1VAR,     /* Both */
  FEATURE_STAT_2VAR,     /* Both */
  FEATURE_STAT_LIN_REG,  /* Both */
  FEATURE_STAT_FORECAST, /* Pro only */

  /* Other Features */
  FEATURE_DATE_CALC,   /* Both */
  FEATURE_BREAKEVEN,   /* Pro only */
  FEATURE_MEMORY_PLUS, /* Pro only - 10 memories vs 1 */

  FEATURE_COUNT /* Total number of features */
} FeatureID;

/* ============================================================
 * Feature Info Structure
 * ============================================================ */
typedef struct {
  FeatureID id;
  const char *name;        /* Display name */
  const char *description; /* Short description */
  int standardMode;        /* 1 if available in Standard */
  int professionalMode;    /* 1 if available in Professional */
} FeatureInfo;

/* ============================================================
 * Feature Checking Functions
 * ============================================================ */

/**
 * Check if a feature is available in the current mode.
 */
int feature_is_available(CalculatorModel model, FeatureID feature);

/**
 * Get feature info by ID.
 */
const FeatureInfo *feature_get_info(FeatureID feature);

/**
 * Get the name of a feature.
 */
const char *feature_get_name(FeatureID feature);

/**
 * Check if feature is Pro-only.
 */
int feature_is_pro_only(FeatureID feature);

/* ============================================================
 * Mode Management
 * ============================================================ */

/**
 * Get current model name.
 */
const char *model_get_name(CalculatorModel model);

/**
 * Get short model indicator (for status bar).
 */
const char *model_get_indicator(CalculatorModel model);

/**
 * Toggle between Standard and Professional mode.
 */
CalculatorModel model_toggle(CalculatorModel current);

/* ============================================================
 * Feature Comparison (for help screens)
 * ============================================================ */

/**
 * Get list of features available only in Professional mode.
 * Returns number of Pro-only features.
 */
int feature_list_pro_only(FeatureID *buffer, int maxCount);

/**
 * Get list of features available in both modes.
 */
int feature_list_common(FeatureID *buffer, int maxCount);

#endif /* FEATURES_H */
