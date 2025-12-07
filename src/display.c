/**
 * Open fx-BA: TI BA II Plus Clone
 * display.c - TI-style prompted display implementation
 *
 * Creates authentic TI BA II Plus look:
 * +--------------------------------+
 * | STD          BGN      2nd      |  <- Status bar
 * |--------------------------------|
 * | N=                             |  <- Variable prompt
 * |                   1,234.56     |  <- Value (right-aligned)
 * |                                |
 * | COMPUTE                    *   |  <- Indicators
 * |--------------------------------|
 * | N  I/Y  PV  PMT  FV  CPT      |  <- F-keys
 * +--------------------------------+
 */

#include "display.h"
#include "config.h"
#include "ui.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

/* Stub for PrintMini if not using fxSDK */
#ifndef USE_FXSDK
#define MINI_OVER 0
#define MINI_REV 1
static void PrintMini(int x, int y, const char *s, int m) {
  (void)x;
  (void)y;
  (void)s;
  (void)m;
}
static void Bdisp_AllClr_VRAM(void) {}
static void Bdisp_PutDisp_DD(void) {}
#endif

/* ============================================================
 * Display Initialization
 * ============================================================ */

void display_init(DisplayState *state) {
  memset(state, 0, sizeof(DisplayState));
  state->currentWorksheet = WS_TVM;
  strcpy(state->varLabel, "0");
}

/* ============================================================
 * Main Render Function
 * ============================================================ */

void display_render(DisplayState *state, Calculator *calc) {
  Bdisp_AllClr_VRAM();

  /* Draw all sections */
  display_draw_status_bar(state, calc);

  if (state->hasError) {
    /* Show error instead of normal display */
    PrintMini(40, VALUE_Y, get_error_message(state->errorCode), MINI_REV);
  } else {
    display_draw_var_prompt(state->varLabel, state->isComputed);
    display_draw_value(state->varValue, 0);
  }

  display_draw_indicator(state);
  display_draw_fkeys(state);

  Bdisp_PutDisp_DD();
}

/* ============================================================
 * Status Bar
 * ============================================================ */

void display_draw_status_bar(DisplayState *state, Calculator *calc) {
  char buffer[32];

  /* Model indicator (left) */
  const char *model = (calc->model == MODEL_PROFESSIONAL) ? "PRO" : "STD";
  PrintMini(0, STATUS_Y, model, MINI_OVER);

  /* BGN/END indicator (center-left) */
  if (calc->tvm.mode == TVM_BEGIN) {
    PrintMini(30, STATUS_Y, "BGN", MINI_OVER);
  }

  /* 2nd indicator (center-right) */
  if (state->secondActive) {
    PrintMini(70, STATUS_Y, "2nd", MINI_REV);
  }

  /* Worksheet indicator (right) */
  switch (state->currentWorksheet) {
  case WS_TVM:
    break; /* No indicator for TVM */
  case WS_CASH_FLOW:
    PrintMini(100, STATUS_Y, "CF", MINI_OVER);
    break;
  case WS_AMORT:
    PrintMini(100, STATUS_Y, "AM", MINI_OVER);
    break;
  case WS_BOND:
    PrintMini(100, STATUS_Y, "BD", MINI_OVER);
    break;
  case WS_DEPRECIATION:
    PrintMini(100, STATUS_Y, "DP", MINI_OVER);
    break;
  case WS_DATE:
    PrintMini(100, STATUS_Y, "DT", MINI_OVER);
    break;
  case WS_STATISTICS:
    PrintMini(100, STATUS_Y, "ST", MINI_OVER);
    break;
  case WS_BREAKEVEN:
    PrintMini(100, STATUS_Y, "BE", MINI_OVER);
    break;
  default:
    break;
  }

  /* Separator line */
  for (int x = 0; x < DISPLAY_WIDTH; x += 2) {
    PrintMini(x, STATUS_Y + 8, "-", MINI_OVER);
  }

  (void)buffer;
}

/* ============================================================
 * Variable Prompt (N=, I/Y=, etc.)
 * ============================================================ */

void display_draw_var_prompt(const char *label, int isComputed) {
  char buffer[16];

  /* Format: "N=" or "I/Y=" */
  snprintf(buffer, sizeof(buffer), "%s=", label);

  /* Draw at left side of screen */
  PrintMini(0, LABEL_Y, buffer, MINI_OVER);

  /* Show asterisk if value was computed */
  if (isComputed) {
    PrintMini(DISPLAY_WIDTH - 8, LABEL_Y, "*", MINI_OVER);
  }
}

/* ============================================================
 * Value Display (Right-Aligned)
 * ============================================================ */

void display_draw_value(double value, int isNegative) {
  char buffer[24];

  /* Format the number */
  if (fabs(value) >= 1e10 || (fabs(value) < 1e-6 && value != 0)) {
    /* Scientific notation for very large/small numbers */
    snprintf(buffer, sizeof(buffer), "%.6e", value);
  } else {
    /* Normal decimal format */
    snprintf(buffer, sizeof(buffer), "%.6f", value);

    /* Remove trailing zeros after decimal */
    int len = strlen(buffer);
    while (len > 1 && buffer[len - 1] == '0' && buffer[len - 2] != '.') {
      buffer[--len] = '\0';
    }
  }

  /* Add negative sign if needed */
  char finalBuffer[26];
  if (isNegative && value >= 0) {
    snprintf(finalBuffer, sizeof(finalBuffer), "-%s", buffer);
  } else {
    strcpy(finalBuffer, buffer);
  }

  /* Calculate right-aligned position */
  int len = strlen(finalBuffer);
  int x = DISPLAY_WIDTH - (len * 6) - 4; /* 6px per char */
  if (x < 4)
    x = 4;

  /* Draw value */
  PrintMini(x, VALUE_Y, finalBuffer, MINI_OVER);
}

/* ============================================================
 * Indicator Line (CPT, COMPUTE, etc.)
 * ============================================================ */

void display_draw_indicator(DisplayState *state) {
  if (state->cptActive) {
    /* Show COMPUTE with optional blink effect */
    int mode = state->cptBlink ? MINI_REV : MINI_OVER;
    PrintMini(0, INDICATOR_Y, "COMPUTE", mode);
  }
}

/* ============================================================
 * F-Key Labels
 * ============================================================ */

void display_draw_fkeys(DisplayState *state) {
  /* Separator line */
  for (int x = 0; x < DISPLAY_WIDTH; x += 2) {
    PrintMini(x, FKEY_Y - 4, "-", MINI_OVER);
  }

  const char *labels[6];
  const char *labels2nd[6];

  switch (state->currentWorksheet) {
  case WS_TVM:
    labels[0] = "N";
    labels[1] = "I/Y";
    labels[2] = "PV";
    labels[3] = "PMT";
    labels[4] = "FV";
    labels[5] = "CPT";
    labels2nd[0] = "xP/Y";
    labels2nd[1] = "P/Y";
    labels2nd[2] = "AMORT";
    labels2nd[3] = "BGN";
    labels2nd[4] = "CLR";
    labels2nd[5] = "QUIT";
    break;
  case WS_CASH_FLOW:
    labels[0] = "CF0";
    labels[1] = "C01";
    labels[2] = "NPV";
    labels[3] = "IRR";
    labels[4] = "↑↓";
    labels[5] = "CPT";
    labels2nd[0] = "";
    labels2nd[1] = "F01";
    labels2nd[2] = "NFV";
    labels2nd[3] = "MIRR";
    labels2nd[4] = "INS";
    labels2nd[5] = "CLR";
    break;
  case WS_AMORT:
    labels[0] = "P1";
    labels[1] = "P2";
    labels[2] = "BAL";
    labels[3] = "PRN";
    labels[4] = "INT";
    labels[5] = "↓";
    labels2nd[0] = "";
    labels2nd[1] = "";
    labels2nd[2] = "";
    labels2nd[3] = "";
    labels2nd[4] = "";
    labels2nd[5] = "";
    break;
  case WS_BOND:
    labels[0] = "SDT";
    labels[1] = "CPN";
    labels[2] = "RDT";
    labels[3] = "PRI";
    labels[4] = "YLD";
    labels[5] = "CPT";
    labels2nd[0] = "";
    labels2nd[1] = "";
    labels2nd[2] = "";
    labels2nd[3] = "AI";
    labels2nd[4] = "DUR";
    labels2nd[5] = "SET";
    break;
  case WS_DEPRECIATION:
    labels[0] = "LIF";
    labels[1] = "CST";
    labels[2] = "SAL";
    labels[3] = "YR";
    labels[4] = "DEP";
    labels[5] = "SET";
    labels2nd[0] = "M01";
    labels2nd[1] = "";
    labels2nd[2] = "";
    labels2nd[3] = "";
    labels2nd[4] = "RBV";
    labels2nd[5] = "";
    break;
  case WS_DATE:
    labels[0] = "DT1";
    labels[1] = "DT2";
    labels[2] = "DBD";
    labels[3] = "ACT";
    labels[4] = "360";
    labels[5] = "CPT";
    labels2nd[0] = "";
    labels2nd[1] = "";
    labels2nd[2] = "";
    labels2nd[3] = "";
    labels2nd[4] = "";
    labels2nd[5] = "";
    break;
  case WS_STATISTICS:
    labels[0] = "X";
    labels[1] = "Y";
    labels[2] = "n";
    labels[3] = "x̄";
    labels[4] = "Sx";
    labels[5] = "→";
    labels2nd[0] = "";
    labels2nd[1] = "";
    labels2nd[2] = "";
    labels2nd[3] = "ȳ";
    labels2nd[4] = "Sy";
    labels2nd[5] = "REG";
    break;
  case WS_BREAKEVEN:
    labels[0] = "FC";
    labels[1] = "VC";
    labels[2] = "P";
    labels[3] = "Q";
    labels[4] = "PFT";
    labels[5] = "CPT";
    labels2nd[0] = "";
    labels2nd[1] = "";
    labels2nd[2] = "";
    labels2nd[3] = "";
    labels2nd[4] = "";
    labels2nd[5] = "";
    break;
  default:
    for (int i = 0; i < 6; i++) {
      labels[i] = "";
      labels2nd[i] = "";
    }
    break;
  }

  /* Draw labels */
  int spacing = DISPLAY_WIDTH / 6;
  for (int i = 0; i < 6; i++) {
    int x = (i * spacing) + 2;

    if (state->secondActive && labels2nd[i][0] != '\0') {
      PrintMini(x, FKEY_Y, labels2nd[i], MINI_REV);
    } else {
      PrintMini(x, FKEY_Y, labels[i], MINI_OVER);
    }
  }
}

/* ============================================================
 * CPT Indicator Functions
 * ============================================================ */

void display_show_cpt(DisplayState *state) {
  state->cptActive = 1;
  state->cptBlink = 0;
}

void display_clear_cpt(DisplayState *state) {
  state->cptActive = 0;
  state->cptBlink = 0;
}

void display_toggle_cpt_blink(DisplayState *state) {
  if (state->cptActive) {
    state->cptBlink = !state->cptBlink;
  }
}

/* ============================================================
 * 2nd Key Functions
 * ============================================================ */

void display_toggle_2nd(DisplayState *state) {
  state->secondActive = !state->secondActive;
}

void display_clear_2nd(DisplayState *state) { state->secondActive = 0; }

/* ============================================================
 * Error Display Functions
 * ============================================================ */

void display_show_error(DisplayState *state, int errorCode) {
  state->hasError = 1;
  state->errorCode = errorCode;
}

void display_clear_error(DisplayState *state) {
  state->hasError = 0;
  state->errorCode = 0;
}

/* ============================================================
 * Variable Update Functions
 * ============================================================ */

static const char *TVM_VAR_LABELS[] = {"N", "I/Y", "PV", "PMT", "FV"};

void display_update_tvm(DisplayState *state, Calculator *calc, TVMVariable var,
                        int computed) {
  if (var >= TVM_VAR_COUNT)
    return;

  strcpy(state->varLabel, TVM_VAR_LABELS[var]);
  state->isComputed = computed;

  switch (var) {
  case TVM_VAR_N:
    state->varValue = calc->tvm.N;
    break;
  case TVM_VAR_IY:
    state->varValue = calc->tvm.I_Y;
    break;
  case TVM_VAR_PV:
    state->varValue = calc->tvm.PV;
    break;
  case TVM_VAR_PMT:
    state->varValue = calc->tvm.PMT;
    break;
  case TVM_VAR_FV:
    state->varValue = calc->tvm.FV;
    break;
  default:
    break;
  }
}

void display_update_cf(DisplayState *state, Calculator *calc, int cfIndex,
                       int isFreq) {
  char label[8];

  if (cfIndex == 0) {
    strcpy(state->varLabel, "CF0");
    state->varValue = calc->cashflow.CF0;
  } else {
    if (isFreq) {
      snprintf(label, sizeof(label), "F%02d", cfIndex);
      state->varValue = (double)calc->cashflow.flows[cfIndex - 1].frequency;
    } else {
      snprintf(label, sizeof(label), "C%02d", cfIndex);
      state->varValue = calc->cashflow.flows[cfIndex - 1].amount;
    }
    strcpy(state->varLabel, label);
  }

  state->isComputed = 0;
}

void display_update_result(DisplayState *state, const char *label,
                           double value) {
  strcpy(state->varLabel, label);
  state->varValue = value;
  state->isComputed = 1;
}

/* ============================================================
 * Worksheet Screen Functions (Simplified)
 * ============================================================ */

void display_draw_tvm_worksheet(Calculator *calc, int currentVar) {
  DisplayState state;
  display_init(&state);
  state.currentWorksheet = WS_TVM;
  display_update_tvm(&state, calc, (TVMVariable)currentVar, 0);
  display_render(&state, calc);
}

void display_draw_cf_worksheet(Calculator *calc, int cfIndex, int showFreq) {
  DisplayState state;
  display_init(&state);
  state.currentWorksheet = WS_CASH_FLOW;
  display_update_cf(&state, calc, cfIndex, showFreq);
  display_render(&state, calc);
}

void display_draw_amort_worksheet(Calculator *calc, int p1, int p2) {
  (void)calc;
  (void)p1;
  (void)p2;
  /* To be implemented with full amort display */
}

void display_draw_bond_worksheet(Calculator *calc, int currentField) {
  (void)calc;
  (void)currentField;
  /* To be implemented with bond fields */
}

void display_draw_depr_worksheet(Calculator *calc, int year,
                                 DepreciationMethod method) {
  (void)calc;
  (void)year;
  (void)method;
  /* To be implemented with depreciation display */
}

void display_draw_stat_worksheet(Calculator *calc, int showResults) {
  (void)calc;
  (void)showResults;
  /* To be implemented with statistics display */
}

void display_draw_breakeven_worksheet(Calculator *calc, int currentField) {
  (void)calc;
  (void)currentField;
  /* To be implemented with breakeven fields */
}
