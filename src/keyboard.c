/**
 * Open fx-BA: TI BA II Plus Clone
 * keyboard.c - TI-style keyboard handling implementation
 *
 * Implements TI BA II Plus key behavior:
 * - [Number] -> [F-key]: Store value
 * - [CPT] -> [F-key]: Compute value
 * - [2nd/SHIFT] -> [F-key]: Access 2nd function
 * - Arrow keys for worksheet navigation
 */

#include "keyboard.h"
#include "config.h"
#include "input.h"
#include "tvm.h"

/* ============================================================
 * Key Classification
 * ============================================================ */

static int is_digit_key(int keyCode, char *digit) {
  switch (keyCode) {
  case KEY_0:
    *digit = '0';
    return 1;
  case KEY_1:
    *digit = '1';
    return 1;
  case KEY_2:
    *digit = '2';
    return 1;
  case KEY_3:
    *digit = '3';
    return 1;
  case KEY_4:
    *digit = '4';
    return 1;
  case KEY_5:
    *digit = '5';
    return 1;
  case KEY_6:
    *digit = '6';
    return 1;
  case KEY_7:
    *digit = '7';
    return 1;
  case KEY_8:
    *digit = '8';
    return 1;
  case KEY_9:
    *digit = '9';
    return 1;
  default:
    return 0;
  }
}

static int is_fkey(int keyCode, int *index) {
  switch (keyCode) {
  case KEY_F1:
    *index = 0;
    return 1;
  case KEY_F2:
    *index = 1;
    return 1;
  case KEY_F3:
    *index = 2;
    return 1;
  case KEY_F4:
    *index = 3;
    return 1;
  case KEY_F5:
    *index = 4;
    return 1;
  case KEY_F6:
    *index = 5;
    return 1;
  default:
    return 0;
  }
}

/* ============================================================
 * Key Processing (Raw -> Event)
 * ============================================================ */

KeyEvent keyboard_process(int keyCode, ScreenState *state) {
  KeyEvent event = {ACTION_NONE, 0, -1, SCREEN_TVM};
  char digit;
  int fkeyIndex;

  /* Handle SHIFT/2nd key */
  if (keyCode == KEY_SHIFT) {
    state->is2ndActive = !state->is2ndActive;
    event.action = ACTION_2ND;
    return event;
  }

  /* Handle digit keys */
  if (is_digit_key(keyCode, &digit)) {
    event.action = ACTION_DIGIT;
    event.digit = digit;
    /* Clear 2nd state after any non-2nd key */
    state->is2ndActive = 0;
    return event;
  }

  /* Handle decimal point */
  if (keyCode == KEY_DOT) {
    event.action = ACTION_DECIMAL;
    state->is2ndActive = 0;
    return event;
  }

  /* Handle +/- key */
  if (keyCode == KEY_NEG) {
    event.action = ACTION_NEGATIVE;
    state->is2ndActive = 0;
    return event;
  }

  /* Handle DEL */
  if (keyCode == KEY_DEL) {
    if (state->is2ndActive) {
      event.action = ACTION_CLEAR; /* 2nd + DEL = Clear entry */
    } else {
      event.action = ACTION_BACKSPACE;
    }
    state->is2ndActive = 0;
    return event;
  }

  /* Handle AC */
  if (keyCode == KEY_AC) {
    event.action = ACTION_CLEAR_ALL;
    state->is2ndActive = 0;
    return event;
  }

  /* Handle F-keys */
  if (is_fkey(keyCode, &fkeyIndex)) {
    event.fkeyIndex = fkeyIndex;

    /* F6 is CPT in TVM mode */
    if (fkeyIndex == 5 && state->currentScreen == SCREEN_TVM) {
      if (state->is2ndActive) {
        /* 2nd + CPT = switch screen or special function */
        event.action = ACTION_SWITCH_SCREEN;
      } else {
        event.action = ACTION_CPT;
        state->isCPTActive = 1;
      }
    } else {
      /* Other F-keys store or compute based on CPT state */
      if (state->isCPTActive) {
        event.action = ACTION_COMPUTE_VAR;
        state->isCPTActive = 0; /* Reset CPT after use */
      } else {
        event.action = ACTION_STORE_VAR;
      }
    }

    if (event.action != ACTION_CPT) {
      state->is2ndActive = 0;
    }
    return event;
  }

  /* Handle EXE */
  if (keyCode == KEY_EXE) {
    event.action = ACTION_ENTER;
    state->is2ndActive = 0;
    return event;
  }

  /* Handle arrow keys */
  if (keyCode == KEY_UP) {
    event.action = ACTION_UP;
    return event;
  }
  if (keyCode == KEY_DOWN) {
    event.action = ACTION_DOWN;
    return event;
  }
  if (keyCode == KEY_LEFT) {
    event.action = ACTION_LEFT;
    return event;
  }
  if (keyCode == KEY_RIGHT) {
    event.action = ACTION_RIGHT;
    return event;
  }

  /* Handle EXIT */
  if (keyCode == KEY_EXIT) {
    event.action = ACTION_EXIT;
    return event;
  }

  /* Handle STO key (mapped to OPTN) */
  if (keyCode == KEY_STO) {
    event.action = ACTION_STO;
    state->is2ndActive = 0;
    return event;
  }

  /* Handle RCL key (mapped to VARS) */
  if (keyCode == KEY_RCL) {
    event.action = ACTION_RCL;
    state->is2ndActive = 0;
    return event;
  }

  return event;
}

/* ============================================================
 * Event Handling
 * ============================================================ */

/* Map F-key index to TVM variable */
static TVMVariable fkey_to_tvm_var(int fkeyIndex) {
  switch (fkeyIndex) {
  case 0:
    return TVM_VAR_N;
  case 1:
    return TVM_VAR_IY;
  case 2:
    return TVM_VAR_PV;
  case 3:
    return TVM_VAR_PMT;
  case 4:
    return TVM_VAR_FV;
  default:
    return TVM_VAR_N;
  }
}

int keyboard_handle_event(KeyEvent *event, Calculator *calc,
                          ScreenState *state) {
  
  /* First, check if we're in error state - any key clears it */
  if (error_is_active(calc) && event->action != ACTION_NONE) {
    error_clear(calc);
    /* Don't process the key further - just clear error */
    return 0;
  }

  /* Check if we're waiting for STO/RCL digit */
  if (calc->state == STATE_WAIT_STO || calc->state == STATE_WAIT_RCL) {
    if (event->action == ACTION_DIGIT) {
      /* Convert digit char to int */
      int digit = event->digit - '0';
      state_handle_memory_digit(calc, digit);
      return 0;
    }
    /* Any other key cancels STO/RCL */
    state_cancel_sto_rcl(calc);
    /* Fall through to handle the key normally */
  }

  switch (event->action) {
  case ACTION_DIGIT:
    input_append_digit(calc, event->digit);
    break;

  case ACTION_DECIMAL:
    input_append_decimal(calc);
    break;

  case ACTION_NEGATIVE:
    input_toggle_negative(calc);
    break;

  case ACTION_BACKSPACE:
    input_backspace(calc);
    break;

  case ACTION_CLEAR:
    input_clear(calc);
    break;

  case ACTION_CLEAR_ALL:
    calc_reset_tvm(calc);
    break;

  case ACTION_CPT:
    state->isCPTActive = 1;
    calc->state = STATE_COMPUTE;
    calc->isComputeActive = 1;
    break;

  case ACTION_2ND:
    calc->is2ndActive = state->is2ndActive;
    break;

  case ACTION_STO:
    state_handle_sto_key(calc);
    break;

  case ACTION_RCL:
    state_handle_rcl_key(calc);
    break;

  case ACTION_STORE_VAR:
    if (event->fkeyIndex >= 0 && event->fkeyIndex < 5) {
      TVMVariable var = fkey_to_tvm_var(event->fkeyIndex);
      state_handle_var_key(calc, var);
      calc->isComputeActive = 0;
    }
    break;

  case ACTION_COMPUTE_VAR:
    if (event->fkeyIndex >= 0 && event->fkeyIndex < 5) {
      calc->state = STATE_COMPUTE;
      TVMVariable var = fkey_to_tvm_var(event->fkeyIndex);
      state_handle_var_key(calc, var);
      calc->isComputeActive = 0;
    }
    break;

  case ACTION_UP:
    if (state->selectedEntry > 0) {
      state->selectedEntry--;
    }
    calc->worksheetIndex = state->selectedEntry;
    break;

  case ACTION_DOWN:
    state->selectedEntry++;
    calc->worksheetIndex = state->selectedEntry;
    break;

  case ACTION_ENTER:
    /* Confirm current entry in worksheet mode */
    break;

  case ACTION_SWITCH_SCREEN:
    /* Cycle through screens */
    state->currentScreen = (state->currentScreen + 1) % 5;
    state->selectedEntry = 0;
    calc->worksheetIndex = 0;
    break;

  case ACTION_EXIT:
    return 1; /* Signal to exit */

  default:
    break;
  }

  return 0;
}

/* ============================================================
 * Helper Function
 * ============================================================ */

int keyboard_is_shift_pressed(void) {
  /* Would check actual key state in fxSDK */
  return 0;
}
