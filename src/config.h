/**
 * Open fx-BA: TI BA II Plus Clone
 * config.h - Configuration constants and feature flags
 */

#ifndef CONFIG_H
#define CONFIG_H

/* ============================================================
 * Application Info
 * ============================================================ */
#define APP_NAME "Open fx-BA"
#define APP_VERSION "1.0.0"
#define APP_AUTHOR "aplkalex"

/* ============================================================
 * Screen Dimensions (fx-9750GIII / fx-9860G series)
 * ============================================================ */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

/* Font sizes (for PrintMini) */
#define CHAR_WIDTH 4
#define CHAR_HEIGHT 6

/* Layout constants */
#define STATUS_BAR_Y 0
#define MAIN_DISPLAY_Y 20
#define FKEY_MENU_Y 58

/* ============================================================
 * TVM Calculation Constants (OPTIMIZED)
 * ============================================================ */
#define DEFAULT_P_Y 12.0 /* Default payments per year */
#define DEFAULT_C_Y 12.0 /* Default compounding per year */

/* Newton-Raphson iteration settings
 * Reduced iterations since analytical derivatives converge faster */
#define MAX_ITERATIONS                                                         \
  50 /* Reduced from 100 - analytical derivatives need fewer */
#define TOLERANCE 1e-10
#define INITIAL_GUESS 0.1 /* 10% as starting point for I/Y */

/* ============================================================
 * Input Limits
 * ============================================================ */
#define MAX_DISPLAY_DIGITS 10 /* TI shows 10 digits */
#define MAX_DECIMAL_PLACES 9

/* ============================================================
 * Feature Flags (Pro vs Standard) - use CFG_ prefix to avoid enum collision
 * ============================================================ */

/* Cash Flow features */
#define CFG_FEATURE_NPV 1 /* Both */
#define CFG_FEATURE_IRR 1 /* Both */
#define CFG_FEATURE_NFV 1 /* Pro only */
#define CFG_FEATURE_PB 1  /* Pro only - Payback Period */
#define CFG_FEATURE_DPB 1 /* Pro only - Discounted Payback */

/* Bond features */
#define CFG_FEATURE_BOND_PRICE 1 /* Both */
#define CFG_FEATURE_BOND_YIELD 1 /* Both */
#define CFG_FEATURE_MOD_DUR 1    /* Pro only - Modified Duration */

/* Other Pro features */
#define CFG_FEATURE_MIRR 1      /* Pro only - Modified IRR */
#define CFG_FEATURE_BREAKEVEN 1 /* Pro only */

/* ============================================================
 * Key Code Mappings (SDK-agnostic via HAL)
 * ============================================================ */
#include "hal/hal_keyboard.h"

#define KEY_0 HAL_KEY_0
#define KEY_1 HAL_KEY_1
#define KEY_2 HAL_KEY_2
#define KEY_3 HAL_KEY_3
#define KEY_4 HAL_KEY_4
#define KEY_5 HAL_KEY_5
#define KEY_6 HAL_KEY_6
#define KEY_7 HAL_KEY_7
#define KEY_8 HAL_KEY_8
#define KEY_9 HAL_KEY_9
#define KEY_DOT HAL_KEY_DOT
#define KEY_NEG HAL_KEY_NEG
#define KEY_EXE HAL_KEY_EXE
#define KEY_EXIT HAL_KEY_EXIT
#define KEY_DEL HAL_KEY_DEL
#define KEY_AC HAL_KEY_AC

/* F-keys */
#define KEY_F1 HAL_KEY_F1
#define KEY_F2 HAL_KEY_F2
#define KEY_F3 HAL_KEY_F3
#define KEY_F4 HAL_KEY_F4
#define KEY_F5 HAL_KEY_F5
#define KEY_F6 HAL_KEY_F6

/* Arrow keys */
#define KEY_UP HAL_KEY_UP
#define KEY_DOWN HAL_KEY_DOWN
#define KEY_LEFT HAL_KEY_LEFT
#define KEY_RIGHT HAL_KEY_RIGHT

/* Shift key */
#define KEY_SHIFT HAL_KEY_SHIFT

/* STO/RCL key mappings (using OPTN and VARS keys) */
#define KEY_STO HAL_KEY_OPTN /* OPTN key -> STO */
#define KEY_RCL HAL_KEY_VARS /* VARS key -> RCL */

/* ============================================================
 * Timing Constants
 * ============================================================ */
#define STO_RCL_TIMEOUT_MS 4000 /* ~4 seconds */
#define ERROR_DISPLAY_MS 2000 /* Error message display time */

#endif /* CONFIG_H */
