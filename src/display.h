/**
 * Open fx-BA: TI BA II Plus Clone
 * display.h - TI-style prompted display system
 *
 * Handles the main screen layout with:
 * - Variable prompts (N=, I/Y=, etc.)
 * - CPT indicator with blinking
 * - Error display
 * - Worksheet navigation
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include "types.h"
#include "worksheets.h"

/* ============================================================
 * Screen Layout Constants
 * ============================================================ */
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

/* Layout regions */
#define STATUS_Y 0
#define LABEL_Y 12
#define VALUE_Y 24
#define INDICATOR_Y 40
#define FKEY_Y 56

/* ============================================================
 * Display State
 * ============================================================ */
typedef struct {
  /* Current variable being displayed */
  char varLabel[12];
  double varValue;
  int isComputed; /* Show * for computed values */

  /* CPT state */
  int cptActive;
  int cptBlink; /* For blinking effect */

  /* 2nd key state */
  int secondActive;

  /* Error state */
  int hasError;
  int errorCode;

  /* Current worksheet */
  WorksheetType currentWorksheet;
  int worksheetIndex;
} DisplayState;

/* ============================================================
 * Main Display Functions
 * ============================================================ */

/**
 * Initialize display state.
 */
void display_init(DisplayState *state);

/**
 * Render the complete TI-style screen.
 */
void display_render(DisplayState *state, Calculator *calc);

/**
 * Draw the status bar (top line).
 * Shows: [STD/PRO] [BGN] [2nd] [RAD/DEG]
 */
void display_draw_status_bar(DisplayState *state, Calculator *calc);

/**
 * Draw the variable prompt line.
 * Shows: N=  or  I/Y=  or  PV=  etc.
 */
void display_draw_var_prompt(const char *label, int isComputed);

/**
 * Draw the main value display (right-aligned, large).
 */
void display_draw_value(Calculator *calc, double value, int isNegative);

/**
 * Draw indicator line (CPT, COMPUTE, etc.).
 */
void display_draw_indicator(DisplayState *state);

/**
 * Draw F-key labels at bottom.
 */
void display_draw_fkeys(DisplayState *state);

/* ============================================================
 * CPT Indicator
 * ============================================================ */

/**
 * Show CPT indicator (called when CPT is pressed).
 */
void display_show_cpt(DisplayState *state);

/**
 * Clear CPT indicator (after variable is computed).
 */
void display_clear_cpt(DisplayState *state);

/**
 * Toggle CPT blink state (for animation).
 */
void display_toggle_cpt_blink(DisplayState *state);

/* ============================================================
 * 2nd Key Indicator
 * ============================================================ */

/**
 * Toggle 2nd key state.
 */
void display_toggle_2nd(DisplayState *state);

/**
 * Clear 2nd state.
 */
void display_clear_2nd(DisplayState *state);

/* ============================================================
 * Error Display
 * ============================================================ */

/**
 * Show error on display.
 */
void display_show_error(DisplayState *state, int errorCode);

/**
 * Clear error state.
 */
void display_clear_error(DisplayState *state);

/* ============================================================
 * Variable Update
 * ============================================================ */

/**
 * Update display with current TVM variable.
 */
void display_update_tvm(DisplayState *state, Calculator *calc, TVMVariable var,
                        int computed);

/**
 * Update display with cash flow variable.
 */
void display_update_cf(DisplayState *state, Calculator *calc, int cfIndex,
                       int isFreq);

/**
 * Update display with computed result.
 */
void display_update_result(DisplayState *state, const char *label,
                           double value);

/* ============================================================
 * Worksheet Screens
 * ============================================================ */

/**
 * Draw TVM worksheet entry screen.
 */
void display_draw_tvm_worksheet(Calculator *calc, int currentVar);

/**
 * Draw Cash Flow worksheet.
 */
void display_draw_cf_worksheet(Calculator *calc, int cfIndex, int showFreq);

/**
 * Draw Amortization results.
 */
void display_draw_amort_worksheet(Calculator *calc, int p1, int p2);

/**
 * Draw Bond worksheet.
 */
void display_draw_bond_worksheet(Calculator *calc, int currentField);

/**
 * Draw Depreciation worksheet.
 */
void display_draw_depr_worksheet(Calculator *calc, int year,
                                 DepreciationMethod method);

/**
 * Draw Statistics worksheet.
 */
void display_draw_stat_worksheet(Calculator *calc, int showResults);

/**
 * Draw Breakeven worksheet.
 */
void display_draw_breakeven_worksheet(Calculator *calc, int currentField);

#endif /* DISPLAY_H */
