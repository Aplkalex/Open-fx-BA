/**
 * Open fx-BA: TI BA II Plus Clone
 * input.c - Input handling and state machine implementation
 */

#include "input.h"
#include "config.h"
#include "hal/hal_system.h"
#include "tvm.h"
#include "ui.h"
#include <stdlib.h>
#include <string.h>

/* ============================================================
 * Calculator Initialization
 * ============================================================ */
void calc_init(Calculator *calc, CalculatorModel model) {
  memset(calc, 0, sizeof(Calculator));

  calc->model = model;
  calc->state = STATE_INPUT;

  /* Initialize TVM defaults */
  calc->tvm.N = 0.0;
  calc->tvm.I_Y = 0.0;
  calc->tvm.PV = 0.0;
  calc->tvm.PMT = 0.0;
  calc->tvm.FV = 0.0;
  calc->tvm.P_Y = DEFAULT_P_Y;
  calc->tvm.C_Y = DEFAULT_C_Y;
  calc->tvm.mode = TVM_END;

  /* Initialize cash flow */
  calc->cashflow.CF0 = 0.0;
  calc->cashflow.count = 0;

  /* Initialize memory registers (M0-M9) */
  memory_init(&calc->memory);

  /* Initialize input buffer */
  input_clear(calc);

  /* Initialize display format (floating by default) */
  calc->displayDecimals = -1;

  /* Initialize status flags */
  calc->is2ndActive = 0;
  calc->isComputeActive = 0;
  calc->stateTimeout = 0;
  calc->worksheetIndex = 0;

  calc->errorCode = ERR_NONE;
  calc->errorMessage[0] = '\0';
}

void calc_reset_tvm(Calculator *calc) {
  calc->tvm.N = 0.0;
  calc->tvm.I_Y = 0.0;
  calc->tvm.PV = 0.0;
  calc->tvm.PMT = 0.0;
  calc->tvm.FV = 0.0;
  /* Keep P/Y, C/Y, and mode as they are */

  input_clear(calc);
  calc->state = STATE_INPUT;
}

void calc_reset_cashflow(Calculator *calc) {
  calc->cashflow.CF0 = 0.0;
  calc->cashflow.count = 0;
  memset(calc->cashflow.flows, 0, sizeof(calc->cashflow.flows));
}

void calc_reset_bond(Calculator *calc) {
  calc->bond.settlementDate = 0;
  calc->bond.couponRate = 0.0;
  calc->bond.maturityDate = 0;
  calc->bond.callDate = 0;
  calc->bond.callPrice = 0.0;
  calc->bond.redemption = 100.0; /* Default to par */
  calc->bond.frequency = 2;      /* Default to semi-annual */
  calc->bond.dayCount = 0;       /* Default to ACT */
  calc->bond.price = 0.0;
  calc->bond.yield = 0.0;
  calc->bond.bondType = 0; /* Default to YTM */
  calc->bond.accruedInterest = 0.0;
  calc->bond.duration = 0.0;
  calc->bond.modDuration = 0.0;
}

void calc_reset_depreciation(Calculator *calc) {
  calc->depreciation.cost = 0.0;
  calc->depreciation.salvage = 0.0;
  calc->depreciation.life = 0.0;
  calc->depreciation.dbRate = 200.0; /* Default DB rate */
  calc->depreciation.startMonth = 1;
  calc->depreciation.currentYear = 1;
}

void calc_reset_statistics(Calculator *calc) {
  memset(&calc->statistics, 0, sizeof(calc->statistics));
  calc->statistics.regType = 0; /* Default to linear regression */
}

void calc_reset_breakeven(Calculator *calc) {
  calc->breakeven.fixedCost = 0.0;
  calc->breakeven.variableCost = 0.0;
  calc->breakeven.price = 0.0;
  calc->breakeven.quantity = 0.0;
  calc->breakeven.profit = 0.0;
}

void calc_reset_margin(Calculator *calc) {
  calc->profitMargin.cost = 0.0;
  calc->profitMargin.selling = 0.0;
}

void calc_set_format(Calculator *calc, int decimals) {
  /* Set decimal places: -1 = floating, 0-9 = fixed */
  if (decimals >= -1 && decimals <= 9) {
    calc->displayDecimals = decimals;
  }
}

/* ============================================================
 * Input Buffer Management
 * ============================================================ */
void input_clear(Calculator *calc) {
  memset(calc->inputBuffer, 0, INPUT_BUFFER_SIZE);
  calc->inputLength = 0;
  calc->hasDecimal = 0;
  calc->isNegative = 0;
}

void input_append_digit(Calculator *calc, char digit) {
  /* If in RESULT state, start fresh input */
  if (calc->state == STATE_RESULT) {
    input_clear(calc);
    calc->state = STATE_INPUT;
  }

  /* If in COMPUTE state, switch to INPUT but don't clear */
  if (calc->state == STATE_COMPUTE) {
    calc->state = STATE_INPUT;
  }

  /* Check buffer limit (leave room for negative sign and decimal) */
  if (calc->inputLength >= INPUT_BUFFER_SIZE - 2) {
    return;
  }

  /* Don't allow leading zeros (except for "0.") */
  if (calc->inputLength == 0 && digit == '0') {
    calc->inputBuffer[calc->inputLength++] = digit;
    return;
  }

  /* Append the digit */
  calc->inputBuffer[calc->inputLength++] = digit;
}

void input_append_decimal(Calculator *calc) {
  if (calc->hasDecimal)
    return; /* Already has decimal */

  if (calc->state == STATE_RESULT) {
    input_clear(calc);
    calc->state = STATE_INPUT;
  }

  /* If empty, add leading zero */
  if (calc->inputLength == 0) {
    calc->inputBuffer[calc->inputLength++] = '0';
  }

  if (calc->inputLength < INPUT_BUFFER_SIZE - 1) {
    calc->inputBuffer[calc->inputLength++] = '.';
    calc->hasDecimal = 1;
  }
}

void input_toggle_negative(Calculator *calc) {
  calc->isNegative = !calc->isNegative;
}

void input_backspace(Calculator *calc) {
  if (calc->inputLength > 0) {
    char removed = calc->inputBuffer[--calc->inputLength];
    calc->inputBuffer[calc->inputLength] = '\0';

    if (removed == '.') {
      calc->hasDecimal = 0;
    }
  }
}

double input_get_value(Calculator *calc) {
  if (calc->inputLength == 0) {
    return 0.0;
  }

  double value = atof(calc->inputBuffer);

  if (calc->isNegative) {
    value = -value;
  }

  return value;
}

/* ============================================================
 * State Machine
 * ============================================================ */
void state_set(Calculator *calc, AppState newState) { calc->state = newState; }

void state_handle_var_key(Calculator *calc, TVMVariable var) {
  if (calc->state == STATE_COMPUTE) {
    /* Compute mode: Calculate the variable */
    double result = tvm_solve_for(calc, var);

    if (calc->errorCode == ERR_NONE) {
      /* Store the result */
      tvm_store(calc, var, result);

      /* Display the result */
      format_number(result, calc->inputBuffer, INPUT_BUFFER_SIZE);
      calc->inputLength = strlen(calc->inputBuffer);
      calc->state = STATE_RESULT;
      calc->isComputeActive = 0;
    } else {
      /* Use new error handling - shows "Error" but keeps TVM values */
      error_set(calc, calc->errorCode, NULL);
    }
  } else {
    /* Input mode: Store the input value */
    if (calc->inputLength > 0) {
      double value = input_get_value(calc);
      tvm_store(calc, var, value);
    }

    /* Clear input after storing */
    input_clear(calc);

    /* Show the stored value */
    double stored = tvm_recall(calc, var);
    format_number(stored, calc->inputBuffer, INPUT_BUFFER_SIZE);
    calc->inputLength = strlen(calc->inputBuffer);
    calc->state = STATE_RESULT;
  }
}

void state_handle_cpt_key(Calculator *calc) { calc->state = STATE_COMPUTE; }

/* ============================================================
 * TVM Variable Access
 * ============================================================ */
void tvm_store(Calculator *calc, TVMVariable var, double value) {
  switch (var) {
  case TVM_VAR_N:
    calc->tvm.N = value;
    break;
  case TVM_VAR_IY:
    calc->tvm.I_Y = value;
    break;
  case TVM_VAR_PV:
    calc->tvm.PV = value;
    break;
  case TVM_VAR_PMT:
    calc->tvm.PMT = value;
    break;
  case TVM_VAR_FV:
    calc->tvm.FV = value;
    break;
  default:
    break;
  }
}

double tvm_recall(Calculator *calc, TVMVariable var) {
  switch (var) {
  case TVM_VAR_N:
    return calc->tvm.N;
  case TVM_VAR_IY:
    return calc->tvm.I_Y;
  case TVM_VAR_PV:
    return calc->tvm.PV;
  case TVM_VAR_PMT:
    return calc->tvm.PMT;
  case TVM_VAR_FV:
    return calc->tvm.FV;
  default:
    return 0.0;
  }
}

/* ============================================================
 * STO/RCL Memory State Handling
 * ============================================================ */

/**
 * Handle STO key press - enter wait state for memory register digit.
 * TI BA II Plus style: STO -> 0-9
 */
void state_handle_sto_key(Calculator *calc) {
  /* Clear any existing error first */
  if (calc->state == STATE_ERROR) {
    error_clear(calc);
  }

  calc->state = STATE_WAIT_STO;
  calc->stateTimeout = hal_system_get_time_ms() + STO_RCL_TIMEOUT_MS;
}

/**
 * Handle RCL key press - enter wait state for memory register digit.
 * TI BA II Plus style: RCL -> 0-9
 */
void state_handle_rcl_key(Calculator *calc) {
  /* Clear any existing error first */
  if (calc->state == STATE_ERROR) {
    error_clear(calc);
  }

  calc->state = STATE_WAIT_RCL;
  calc->stateTimeout = hal_system_get_time_ms() + STO_RCL_TIMEOUT_MS;
}

/**
 * Handle digit key when in STO or RCL wait state.
 * @param digit Memory register index (0-9)
 */
void state_handle_memory_digit(Calculator *calc, int digit) {
  if (digit < 0 || digit > 9) {
    state_cancel_sto_rcl(calc);
    return;
  }

  if (calc->state == STATE_WAIT_STO) {
    /* Store current display value to memory register */
    double value = input_get_value(calc);
    memory_store(&calc->memory, digit, value);

    /* Brief feedback: show what was stored */
    /* (The UI will show "M#=value" briefly) */
    calc->state = STATE_RESULT;
    calc->stateTimeout = 0;
  } else if (calc->state == STATE_WAIT_RCL) {
    /* Recall value from memory register to display */
    double value = memory_recall(&calc->memory, digit);

    /* Update display buffer with recalled value */
    format_number(value, calc->inputBuffer, INPUT_BUFFER_SIZE);
    calc->inputLength = strlen(calc->inputBuffer);
    calc->isNegative = (value < 0) ? 1 : 0;
    calc->hasDecimal = (strchr(calc->inputBuffer, '.') != NULL) ? 1 : 0;

    calc->state = STATE_RESULT;
    calc->stateTimeout = 0;
  }
}

/**
 * Check and decrement timeout counter for STO/RCL states.
 * Call this periodically (e.g., every 100ms) from main loop.
 * Auto-cancels STO/RCL if timeout expires.
 */
void state_check_timeout(Calculator *calc) {
  if (calc->state != STATE_WAIT_STO && calc->state != STATE_WAIT_RCL)
    return;

  if (calc->stateTimeout == 0)
    return;

  unsigned long now = hal_system_get_time_ms();
  if (now >= calc->stateTimeout) {
    /* Timeout expired - cancel STO/RCL and return to normal */
    state_cancel_sto_rcl(calc);
  }
}

/**
 * Cancel STO/RCL wait state and return to normal input.
 */
void state_cancel_sto_rcl(Calculator *calc) {
  if (calc->state == STATE_WAIT_STO || calc->state == STATE_WAIT_RCL) {
    calc->state = STATE_INPUT;
    calc->stateTimeout = 0;
  }
}

/* ============================================================
 * Error Handling (TI BA II Plus style)
 *
 * Key principle: Show "Error" in display, but DON'T clear
 * the user's TVM/memory values. User can press any key to
 * clear error and continue.
 * ============================================================ */

/**
 * Set error state with code and display message.
 * Does NOT clear TVM or memory values.
 */
void error_set(Calculator *calc, int errorCode, const char *message) {
  calc->errorCode = errorCode;
  calc->state = STATE_ERROR;

  if (message != NULL) {
    strncpy(calc->errorMessage, message, sizeof(calc->errorMessage) - 1);
    calc->errorMessage[sizeof(calc->errorMessage) - 1] = '\0';
  } else {
    /* Default error messages based on code - differentiated for better UX */
    switch (errorCode) {
    case ERR_NO_SOLUTION:
      strcpy(calc->errorMessage, "No Solution");
      break;
    case ERR_OVERFLOW:
      strcpy(calc->errorMessage, "Overflow");
      break;
    case ERR_ITERATION:
      strcpy(calc->errorMessage, "No Converge");
      break;
    case ERR_INVALID_INPUT:
      strcpy(calc->errorMessage, "Bad Input");
      break;
    case ERR_IRR_MULTIPLE:
      strcpy(calc->errorMessage, "Multi IRR");
      break;
    default:
      strcpy(calc->errorMessage, "Error");
      break;
    }
  }

  /* Copy error message to display buffer */
  strcpy(calc->inputBuffer, calc->errorMessage);
  calc->inputLength = strlen(calc->inputBuffer);
}

/**
 * Clear error state and return to input mode.
 * TVM/memory values are preserved.
 */
void error_clear(Calculator *calc) {
  calc->errorCode = ERR_NONE;
  calc->errorMessage[0] = '\0';
  calc->state = STATE_INPUT;

  /* Reset display to 0, but keep TVM values */
  input_clear(calc);
}

/**
 * Check if calculator is in error state.
 */
int error_is_active(Calculator *calc) {
  return (calc->state == STATE_ERROR || calc->errorCode != ERR_NONE);
}
