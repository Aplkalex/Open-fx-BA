/**
 * Open fx-BA: TI BA II Plus Clone
 * worksheets.h - TI-style worksheet definitions for all modules
 *
 * Implements prompted display with variable labels and navigation
 */

#ifndef WORKSHEETS_H
#define WORKSHEETS_H

#include "bond.h"
#include "cashflow.h"
#include "date.h"
#include "depreciation.h"
#include "profit.h"
#include "statistics.h"
#include "tvm.h"
#include "types.h"

/* ============================================================
 * Variable Labels for Prompted Display
 * ============================================================ */

/* TVM Variable Labels */
#define LABEL_N "N"
#define LABEL_IY "I/Y"
#define LABEL_PV "PV"
#define LABEL_PMT "PMT"
#define LABEL_FV "FV"
#define LABEL_PY "P/Y"
#define LABEL_CY "C/Y"

/* Cash Flow Labels */
#define LABEL_CF0 "CF0"
#define LABEL_CFn "C%02d" /* C01, C02, etc. */
#define LABEL_Fn "F%02d"  /* F01, F02, etc. */
#define LABEL_I "I"
#define LABEL_NPV "NPV"
#define LABEL_IRR "IRR"
#define LABEL_NFV "NFV"
#define LABEL_PB "PB"
#define LABEL_DPB "DPB"

/* Bond Labels */
#define LABEL_SDT "SDT"
#define LABEL_CPN "CPN"
#define LABEL_RDT "RDT"
#define LABEL_RV "RV"
#define LABEL_ACT "ACT"
#define LABEL_2Y "2/Y"
#define LABEL_YLD "YLD"
#define LABEL_PRI "PRI"
#define LABEL_AI "AI"
#define LABEL_DUR "DUR"

/* Depreciation Labels */
#define LABEL_LIF "LIF"
#define LABEL_M01 "M01"
#define LABEL_DT1 "DT1"
#define LABEL_CST "CST"
#define LABEL_SAL "SAL"
#define LABEL_YR "YR"
#define LABEL_DEP "DEP"
#define LABEL_RDV "RDV"
#define LABEL_RBV "RBV"

/* Amortization Labels */
#define LABEL_P1 "P1"
#define LABEL_P2 "P2"
#define LABEL_BAL "BAL"
#define LABEL_PRN "PRN"
#define LABEL_INT "INT"

/* Date Labels */
#define LABEL_DT1_DATE "DT1"
#define LABEL_DT2_DATE "DT2"
#define LABEL_DBD "DBD"

/* Statistics Labels */
#define LABEL_X "X"
#define STAT_LABEL_Y "Y"
#define LABEL_XBAR "x̄"
#define LABEL_SX "Sx"
#define LABEL_SIGX "σx"
#define LABEL_N_STAT "n"
#define LABEL_YBAR "ȳ"
#define LABEL_A "a"
#define LABEL_B "b"
#define LABEL_R "r"

/* Breakeven Labels */
#define LABEL_FC "FC"
#define LABEL_VC "VC"
#define LABEL_P "P"
#define LABEL_PFT "PFT"
#define LABEL_Q "Q"

/* ============================================================
 * Worksheet State
 * ============================================================ */
typedef enum {
  WS_TVM,
  WS_CASH_FLOW,
  WS_AMORT,
  WS_BOND,
  WS_DEPRECIATION,
  WS_DATE,
  WS_STATISTICS,
  WS_BREAKEVEN,
  WS_PROFIT_MARGIN
} WorksheetType;

typedef struct {
  WorksheetType type;
  int currentIndex;      /* Current variable index in the worksheet */
  int totalItems;        /* Total items in this worksheet */
  char currentLabel[12]; /* Current variable label being displayed */
  int isComputed;        /* 1 if last value was computed (show * indicator) */
  int lastVarAccessed;   /* Last TVM/other variable that was accessed */
} WorksheetState;

/* ============================================================
 * Worksheet Functions
 * ============================================================ */

/**
 * Initialize worksheet state.
 */
void ws_init(WorksheetState *ws, WorksheetType type);

/**
 * Move to next item in worksheet.
 */
void ws_next(WorksheetState *ws);

/**
 * Move to previous item in worksheet.
 */
void ws_prev(WorksheetState *ws);

/**
 * Get the label for current item.
 */
const char *ws_get_label(WorksheetState *ws);

/**
 * Get the value for current item.
 */
double ws_get_value(WorksheetState *ws, Calculator *calc);

/**
 * Set the value for current item.
 */
void ws_set_value(WorksheetState *ws, Calculator *calc, double value);

/* ============================================================
 * TVM Worksheet Specifics
 * ============================================================ */
typedef struct {
  int currentVar; /* 0=N, 1=I/Y, 2=PV, 3=PMT, 4=FV */
  int showPY;     /* 1 if showing P/Y settings */
} TVMWorksheet;

void tvm_ws_init(TVMWorksheet *ws);
const char *tvm_ws_get_label(TVMWorksheet *ws);
double tvm_ws_get_value(TVMWorksheet *ws, Calculator *calc);

/* ============================================================
 * Cash Flow Worksheet Specifics
 * ============================================================ */
typedef struct {
  int mode;     /* 0=entry, 1=results */
  int cfIndex;  /* Current CF index (0=CF0, 1-32=C01-C32) */
  int showFreq; /* 1 if showing F (frequency), 0 if showing C (amount) */
} CFWorksheet;

void cf_ws_init(CFWorksheet *ws);
const char *cf_ws_get_label(CFWorksheet *ws, char *buffer);

/* ============================================================
 * Amortization Worksheet Specifics
 * ============================================================ */
typedef struct {
  int p1;         /* Starting period */
  int p2;         /* Ending period */
  int showResult; /* 0=input, 1=BAL, 2=PRN, 3=INT */
} AmortWorksheet;

void amort_ws_init(AmortWorksheet *ws);

/* ============================================================
 * Error Display
 * ============================================================ */

/**
 * Get error message for error code.
 */
const char *get_error_message(int errorCode);

/**
 * Display error with blinking or highlight.
 */
void display_error(int errorCode);

/**
 * Get navigation hint string for current position (e.g., "[^v]")
 */
const char* ws_get_nav_hint(WorksheetState *ws);

#endif /* WORKSHEETS_H */
