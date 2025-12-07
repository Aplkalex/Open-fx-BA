/**
 * Open fx-BA: TI BA II Plus Clone
 * worksheets.c - TI-style worksheet implementation
 *
 * Implements prompted display with "N=", "I/Y=" style variable labels
 */

#include "worksheets.h"
#include "config.h"
#include <stdio.h>
#include <string.h>

/* ============================================================
 * Error Messages (TI BA II Plus style - simple)
 * ============================================================ */
static const char *ERROR_MESSAGES[] = {
    "",      /* 0: No error */
    "Error", /* 1: No solution exists */
    "Error", /* 2: Overflow */
    "Error", /* 3: Iteration limit */
    "Error", /* 4: Invalid input */
    "Error", /* 5: Multiple IRR */
    "Error", /* 6: Not enough data */
    "Error", /* 7: Invalid date */
    "Error"  /* 8: Memory full */
};

const char *get_error_message(int errorCode) {
  if (errorCode < 0 || errorCode > 8)
    return "Error";
  return ERROR_MESSAGES[errorCode];
}

/* ============================================================
 * TVM Variable Labels Array
 * ============================================================ */
static const char *TVM_LABELS[] = {"N", "I/Y", "PV", "PMT", "FV"};

/* ============================================================
 * Worksheet Initialization
 * ============================================================ */

void ws_init(WorksheetState *ws, WorksheetType type) {
  ws->type = type;
  ws->currentIndex = 0;
  ws->isComputed = 0;
  ws->lastVarAccessed = -1;
  memset(ws->currentLabel, 0, sizeof(ws->currentLabel));

  switch (type) {
  case WS_TVM:
    ws->totalItems = 5; /* N, I/Y, PV, PMT, FV */
    break;
  case WS_CASH_FLOW:
    ws->totalItems = 4; /* CF0, then dynamic */
    break;
  case WS_AMORT:
    ws->totalItems = 5; /* P1, P2, BAL, PRN, INT */
    break;
  case WS_BOND:
    ws->totalItems = 8; /* SDT, CPN, RDT, RV, YLD, PRI, AI, DUR */
    break;
  case WS_DEPRECIATION:
    ws->totalItems = 7; /* LIF, M01, CST, SAL, YR, DEP, RBV */
    break;
  case WS_DATE:
    ws->totalItems = 3; /* DT1, DT2, DBD */
    break;
  case WS_STATISTICS:
    ws->totalItems = 2; /* X, Y (data entry) */
    break;
  case WS_BREAKEVEN:
    ws->totalItems = 5; /* FC, VC, P, Q, PFT */
    break;
  case WS_PROFIT_MARGIN:
    ws->totalItems = 4; /* CST, SEL, MAR, MU */
    break;
  }
}

void ws_next(WorksheetState *ws) {
  ws->currentIndex++;
  if (ws->currentIndex >= ws->totalItems) {
    ws->currentIndex = 0;
  }
  ws->isComputed = 0;
}

void ws_prev(WorksheetState *ws) {
  ws->currentIndex--;
  if (ws->currentIndex < 0) {
    ws->currentIndex = ws->totalItems - 1;
  }
  ws->isComputed = 0;
}

const char *ws_get_label(WorksheetState *ws) {
  switch (ws->type) {
  case WS_TVM:
    if (ws->currentIndex >= 0 && ws->currentIndex < 5) {
      return TVM_LABELS[ws->currentIndex];
    }
    break;
  default:
    break;
  }
  return "";
}

/* ============================================================
 * TVM Worksheet
 * ============================================================ */

void tvm_ws_init(TVMWorksheet *ws) {
  ws->currentVar = 0;
  ws->showPY = 0;
}

const char *tvm_ws_get_label(TVMWorksheet *ws) {
  if (ws->showPY) {
    return (ws->currentVar == 0) ? "P/Y" : "C/Y";
  }
  return TVM_LABELS[ws->currentVar];
}

double tvm_ws_get_value(TVMWorksheet *ws, Calculator *calc) {
  if (ws->showPY) {
    return (ws->currentVar == 0) ? calc->tvm.P_Y : calc->tvm.C_Y;
  }

  switch (ws->currentVar) {
  case 0:
    return calc->tvm.N;
  case 1:
    return calc->tvm.I_Y;
  case 2:
    return calc->tvm.PV;
  case 3:
    return calc->tvm.PMT;
  case 4:
    return calc->tvm.FV;
  default:
    return 0;
  }
}

/* ============================================================
 * Cash Flow Worksheet
 * ============================================================ */

void cf_ws_init(CFWorksheet *ws) {
  ws->mode = 0;
  ws->cfIndex = 0;
  ws->showFreq = 0;
}

const char *cf_ws_get_label(CFWorksheet *ws, char *buffer) {
  if (ws->cfIndex == 0) {
    return "CF0";
  }

  if (ws->showFreq) {
    sprintf(buffer, "F%02d", ws->cfIndex);
  } else {
    sprintf(buffer, "C%02d", ws->cfIndex);
  }
  return buffer;
}

/* ============================================================
 * Amortization Worksheet
 * ============================================================ */

void amort_ws_init(AmortWorksheet *ws) {
  ws->p1 = 1;
  ws->p2 = 1;
  ws->showResult = 0;
}

/* ============================================================
 * Get/Set Value Helpers
 * ============================================================ */

double ws_get_value(WorksheetState *ws, Calculator *calc) {
  switch (ws->type) {
  case WS_TVM: {
    switch (ws->currentIndex) {
    case 0:
      return calc->tvm.N;
    case 1:
      return calc->tvm.I_Y;
    case 2:
      return calc->tvm.PV;
    case 3:
      return calc->tvm.PMT;
    case 4:
      return calc->tvm.FV;
    }
    break;
  }
  case WS_CASH_FLOW: {
    if (ws->currentIndex == 0) {
      return calc->cashflow.CF0;
    }
    int cfIdx = (ws->currentIndex - 1) / 2;
    int isFreq = (ws->currentIndex - 1) % 2;
    if (cfIdx < calc->cashflow.count) {
      if (isFreq) {
        return (double)calc->cashflow.flows[cfIdx].frequency;
      } else {
        return calc->cashflow.flows[cfIdx].amount;
      }
    }
    break;
  }
  case WS_BOND: {
    /* Bond worksheet: SDT, CPN, RDT, RV, YLD, PRI, AI, DUR */
    switch (ws->currentIndex) {
    case 0:
      return (double)calc->bond.settlementDate;
    case 1:
      return calc->bond.couponRate;
    case 2:
      return (double)calc->bond.maturityDate;
    case 3:
      return calc->bond.redemption;
    case 4:
      return calc->bond.yield;
    case 5:
      return calc->bond.price;
    case 6:
      return 0.0; /* AI - computed, not stored */
    case 7:
      return 0.0; /* Duration - computed, not stored */
    }
    break;
  }
  case WS_DEPRECIATION: {
    /* Depreciation: LIF, MON, CST, SAL, YR, DEP, RBV */
    switch (ws->currentIndex) {
    case 0:
      return calc->depreciation.life;
    case 1:
      return (double)calc->depreciation.startMonth;
    case 2:
      return calc->depreciation.cost;
    case 3:
      return calc->depreciation.salvage;
    case 4:
      return (double)calc->depreciation.currentYear;
    case 5:
      return 0.0; /* DEP - computed */
    case 6:
      return 0.0; /* RBV - computed */
    }
    break;
  }
  case WS_DATE: {
    /* Date: DT1, DT2, DBD */
    switch (ws->currentIndex) {
    case 0:
      return (double)calc->dateWs.dt1;
    case 1:
      return (double)calc->dateWs.dt2;
    case 2:
      return (double)calc->dateWs.dbd;
    }
    break;
  }
  case WS_STATISTICS: {
    /* Statistics: X entry, n, mean, Sx, σx */
    switch (ws->currentIndex) {
    case 0:
      return 0.0; /* Current X entry */
    case 1:
      return (double)calc->statistics.count;
    }
    break;
  }
  default:
    break;
  }
  return 0;
}

void ws_set_value(WorksheetState *ws, Calculator *calc, double value) {
  switch (ws->type) {
  case WS_TVM: {
    switch (ws->currentIndex) {
    case 0:
      calc->tvm.N = value;
      break;
    case 1:
      calc->tvm.I_Y = value;
      break;
    case 2:
      calc->tvm.PV = value;
      break;
    case 3:
      calc->tvm.PMT = value;
      break;
    case 4:
      calc->tvm.FV = value;
      break;
    }
    break;
  }
  case WS_CASH_FLOW: {
    if (ws->currentIndex == 0) {
      calc->cashflow.CF0 = value;
    }
    break;
  }
  case WS_BOND: {
    /* Bond worksheet: SDT, CPN, RDT, RV, YLD, PRI */
    switch (ws->currentIndex) {
    case 0:
      calc->bond.settlementDate = (int)value;
      break;
    case 1:
      calc->bond.couponRate = value;
      break;
    case 2:
      calc->bond.maturityDate = (int)value;
      break;
    case 3:
      calc->bond.redemption = value;
      break;
    case 4:
      calc->bond.yield = value;
      break;
    case 5:
      calc->bond.price = value;
      break;
    }
    break;
  }
  case WS_DEPRECIATION: {
    /* Depreciation: LIF, MON, CST, SAL, YR */
    switch (ws->currentIndex) {
    case 0:
      calc->depreciation.life = value;
      break;
    case 1:
      calc->depreciation.startMonth = (int)value;
      break;
    case 2:
      calc->depreciation.cost = value;
      break;
    case 3:
      calc->depreciation.salvage = value;
      break;
    case 4:
      calc->depreciation.currentYear = (int)value;
      break;
    }
    break;
  }
  case WS_DATE: {
    /* Date: DT1, DT2, DBD */
    switch (ws->currentIndex) {
    case 0:
      calc->dateWs.dt1 = (int)value;
      break;
    case 1:
      calc->dateWs.dt2 = (int)value;
      break;
    case 2:
      calc->dateWs.dbd = (int)value;
      break;
    }
    break;
  }
  case WS_STATISTICS: {
    /* Statistics: add data point */
    if (ws->currentIndex == 0 && calc->statistics.count < 50) {
      calc->statistics.xData[calc->statistics.count++] = value;
    }
    break;
  }
  default:
    break;
  }
}

/* ============================================================
 * Display Error
 * ============================================================ */

void display_error(int errorCode) {
  /*
   * TI BA II Plus style error display:
   * Simply shows "Error" in the display area.
   * Does NOT clear TVM/memory values.
   * User presses any key to clear and continue.
   */
  const char *msg = get_error_message(errorCode);
  /* The actual display happens in UI layer */
  (void)msg;
  (void)errorCode;
}

/* ============================================================
 * TI-Style Navigation Hint Strings
 * ============================================================ */

const char *ws_get_nav_hint(WorksheetState *ws) {
  /* Return appropriate navigation hint based on position */
  if (ws->currentIndex == 0 && ws->totalItems > 1) {
    return "[v]";
  } else if (ws->currentIndex == ws->totalItems - 1 && ws->totalItems > 1) {
    return "[^]";
  } else if (ws->totalItems > 1) {
    return "[^v]";
  }
  return "";
}
