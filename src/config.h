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
 * Key Code Mappings (Casio-specific)
 * ============================================================ */

/* These will be defined based on actual fxSDK key codes */
/* Placeholder values for now */
#define KEY_0 0x7E
#define KEY_1 0x72
#define KEY_2 0x62
#define KEY_3 0x52
#define KEY_4 0x73
#define KEY_5 0x63
#define KEY_6 0x53
#define KEY_7 0x74
#define KEY_8 0x64
#define KEY_9 0x54
#define KEY_DOT 0x61
#define KEY_NEG 0x51
#define KEY_EXE 0x31
#define KEY_EXIT 0x47
#define KEY_DEL 0x44
#define KEY_AC 0x07

/* F-keys */
#define KEY_F1 0x79
#define KEY_F2 0x69
#define KEY_F3 0x59
#define KEY_F4 0x49
#define KEY_F5 0x39
#define KEY_F6 0x29

/* Arrow keys */
#define KEY_UP 0x28
#define KEY_DOWN 0x37
#define KEY_LEFT 0x38
#define KEY_RIGHT 0x27

/* Shift key */
#define KEY_SHIFT 0x78

/* STO/RCL key mappings (using OPTN and VARS keys) */
#define KEY_STO 0x68 /* OPTN key -> STO */
#define KEY_RCL 0x58 /* VARS key -> RCL */

/* ============================================================
 * Timing Constants
 * ============================================================ */
#define STO_RCL_TIMEOUT 40    /* ~4 seconds at 10 ticks/sec */
#define ERROR_DISPLAY_MS 2000 /* Error message display time */

#endif /* CONFIG_H */
