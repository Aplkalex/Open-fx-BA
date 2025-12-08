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
    display_draw_value(calc, state->varValue, 0);
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

  /* Format indicator (after 2nd) */
  if (calc->state == STATE_WAIT_FORMAT) {
    PrintMini(90, STATUS_Y, "FMT>", MINI_REV);
  } else if (calc->displayDecimals >= 0) {
    char fmtBuf[8];
    snprintf(fmtBuf, sizeof(fmtBuf), "D%d", calc->displayDecimals);
    PrintMini(90, STATUS_Y, fmtBuf, MINI_OVER);
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

void display_draw_value(Calculator *calc, double value, int isNegative) {
  char buffer[24];

  /* Format the number based on displayDecimals setting */
  if (fabs(value) >= 1e10 || (fabs(value) < 1e-6 && value != 0)) {
    /* Scientific notation for very large/small numbers */
    snprintf(buffer, sizeof(buffer), "%.6e", value);
  } else {
    /* Apply decimal format setting */
    if (calc->displayDecimals >= 0) {
      /* Fixed decimal places (0-9) */
      snprintf(buffer, sizeof(buffer), "%.*f", calc->displayDecimals, value);
    } else {
      /* Floating mode - up to 10 digits total */
      snprintf(buffer, sizeof(buffer), "%.10g", value);
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
  DisplayState state;
  display_init(&state);
  state.currentWorksheet = WS_AMORT;

  /* Show P1, P2, or results based on field */
  if (p1 == 0) {
    strcpy(state.varLabel, "P1");
    state.varValue = 1;
  } else if (p2 == 0) {
    strcpy(state.varLabel, "P2");
    state.varValue = 1;
  } else {
    strcpy(state.varLabel, "BAL");
    /* Would compute from tvm_amort_range */
    state.varValue = 0;
    state.isComputed = 1;
  }

  (void)calc; /* Silence warning until full implementation */
  display_render(&state, calc);
}

void display_draw_bond_worksheet(Calculator *calc, int currentField) {
  static const char *BOND_LABELS[] = {"SDT", "CPN", "RDT", "CDT", "CPR", "RV",
                                      "FRQ", "DAY", "YLD", "PRI", "AI",  "DUR"};

  DisplayState state;
  display_init(&state);
  state.currentWorksheet = WS_BOND;

  if (currentField >= 0 && currentField < 12) {
    strcpy(state.varLabel, BOND_LABELS[currentField]);
  }

  switch (currentField) {
  case 0: /* SDT - Settlement Date */
    state.varValue = (double)calc->bond.settlementDate;
    break;
  case 1: /* CPN - Coupon Rate */
    state.varValue = calc->bond.couponRate;
    break;
  case 2: /* RDT - Maturity Date */
    state.varValue = (double)calc->bond.maturityDate;
    break;
  case 3: /* CDT - Call Date */
    state.varValue = (double)calc->bond.callDate;
    break;
  case 4: /* CPR - Call Price */
    state.varValue = calc->bond.callPrice;
    break;
  case 5: /* RV - Redemption Value */
    state.varValue = calc->bond.redemption;
    break;
  case 6: /* FRQ - Frequency (1,2,4,12) */
    state.varValue = (double)calc->bond.frequency;
    break;
  case 7: /* DAY - Day Count (0=ACT, 1=360) */
    state.varValue = (double)calc->bond.dayCount;
    break;
  case 8: /* YLD - Yield (YTM or YTC depending on mode) */
    state.varValue = calc->bond.yield;
    break;
  case 9: /* PRI - Price */
    state.varValue = calc->bond.price;
    break;
  case 10: /* AI - Accrued Interest (computed) */
    state.varValue = 0;
    state.isComputed = 1;
    break;
  case 11: /* DUR - Duration (computed) */
    state.varValue = 0;
    state.isComputed = 1;
    break;
  default:
    state.varValue = 0;
    break;
  }

  display_render(&state, calc);
}

void display_draw_depr_worksheet(Calculator *calc, int year,
                                 DepreciationMethod method) {
  static const char *DEPR_LABELS[] = {"MTH", "LIF", "M01", "CST",
                                      "SAL", "YR",  "DEP", "RBV"};

  DisplayState state;
  display_init(&state);
  state.currentWorksheet = WS_DEPRECIATION;

  /* Show current field based on worksheetIndex */
  int field = calc->worksheetIndex;
  if (field >= 0 && field < 8) {
    strcpy(state.varLabel, DEPR_LABELS[field]);
  }

  switch (field) {
  case 0:
    state.varValue = (double)calc->depreciation.method;
    break;
  case 1:
    state.varValue = calc->depreciation.life;
    break;
  case 2:
    state.varValue = (double)calc->depreciation.startMonth;
    break;
  case 3:
    state.varValue = calc->depreciation.cost;
    break;
  case 4:
    state.varValue = calc->depreciation.salvage;
    break;
  case 5:
    state.varValue = (double)year;
    break;
  case 6:
  case 7:
    /* DEP and RBV - computed */
    state.varValue = 0;
    state.isComputed = 1;
    break;
  default:
    state.varValue = 0;
    break;
  }

  (void)method; /* Used in actual calculation */
  display_render(&state, calc);
}

void display_draw_stat_worksheet(Calculator *calc, int showResults) {
  DisplayState state;
  display_init(&state);
  state.currentWorksheet = WS_STATISTICS;

  if (showResults) {
    /* Show computed statistics */
    static const char *STAT_RESULT_LABELS[] = {"n",  "x̄",  "Sx",
                                               "σx", "Σx", "Σx²"};
    int field = calc->worksheetIndex;

    if (field >= 0 && field < 6) {
      strcpy(state.varLabel, STAT_RESULT_LABELS[field]);
    }

    /* Would compute from stat_calc_1var */
    state.varValue = (double)calc->statistics.count;
    state.isComputed = 1;
  } else {
    /* Data entry mode */
    char label[8];
    snprintf(label, sizeof(label), "X%02d", calc->statistics.count + 1);
    strcpy(state.varLabel, label);
    state.varValue = 0;
  }

  display_render(&state, calc);
}

void display_draw_breakeven_worksheet(Calculator *calc, int currentField) {
  static const char *BE_LABELS[] = {"FC", "VC", "P", "Q", "PFT"};

  DisplayState state;
  display_init(&state);
  state.currentWorksheet = WS_BREAKEVEN;

  if (currentField >= 0 && currentField < 5) {
    strcpy(state.varLabel, BE_LABELS[currentField]);
  }

  switch (currentField) {
  case 0:
    state.varValue = calc->breakeven.fixedCost;
    break;
  case 1:
    state.varValue = calc->breakeven.variableCost;
    break;
  case 2:
    state.varValue = calc->breakeven.price;
    break;
  case 3:
    state.varValue = calc->breakeven.quantity;
    state.isComputed = 1;
    break;
  case 4:
    state.varValue = calc->breakeven.profit;
    state.isComputed = 1;
    break;
  default:
    state.varValue = 0;
    break;
  }

  display_render(&state, calc);
}

/* Profit margin worksheet display */
void display_draw_margin_worksheet(Calculator *calc, int currentField) {
  static const char *PM_LABELS[] = {"CST", "SEL", "MAR", "MU"};

  DisplayState state;
  display_init(&state);
  state.currentWorksheet = WS_PROFIT_MARGIN;

  if (currentField >= 0 && currentField < 4) {
    strcpy(state.varLabel, PM_LABELS[currentField]);
  }

  switch (currentField) {
  case 0:
    state.varValue = calc->profitMargin.cost;
    break;
  case 1:
    state.varValue = calc->profitMargin.selling;
    break;
  case 2:
    state.varValue = calc->profitMargin.margin;
    state.isComputed = 1;
    break;
  case 3:
    state.varValue = calc->profitMargin.markup;
    state.isComputed = 1;
    break;
  default:
    state.varValue = 0;
    break;
  }

  display_render(&state, calc);
}
