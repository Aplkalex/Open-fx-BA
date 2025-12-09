/**
 * Open fx-BA: TI BA II Plus Clone
 * main.c - Application entry point and event loop
 *
 * For Casio fx-9750GIII / fx-9860G series
 * Using fxSDK / gint library
 */

#include "cashflow.h"
#include "config.h"
#include "features.h"
#include "input.h"
#include "screens.h"
#include "tests.h"
#include "tvm.h"
#include "types.h"
#include "ui.h"

#include "hal/hal_keyboard.h"
#include "hal/hal_system.h"

#ifdef USE_CASIO_SDK
#include <fxlib.h>
#endif

#include <stdio.h>
#include <string.h>

/* ============================================================
 * Global Calculator State
 * ============================================================ */
static Calculator calc;

/* ============================================================
 * Key Handling
 * ============================================================ */

/**
 * Check if a key is a digit (0-9)
 */
static int is_digit_key(HAL_Key key, char *digit) {
  switch (key) {
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

/**
 * Map F-key to TVM variable
 */
static int fkey_to_tvm(HAL_Key key, TVMVariable *var) {
  switch (key) {
  case KEY_F1:
    *var = TVM_VAR_N;
    return 1;
  case KEY_F2:
    *var = TVM_VAR_IY;
    return 1;
  case KEY_F3:
    *var = TVM_VAR_PV;
    return 1;
  case KEY_F4:
    *var = TVM_VAR_PMT;
    return 1;
  case KEY_F5:
    *var = TVM_VAR_FV;
    return 1;
  default:
    return 0;
  }
}

/**
 * Process a single key press
 * Returns 0 to continue, 1 to exit
 */
__attribute__((unused)) static int process_key(HAL_Key key) {
  char digit;
  TVMVariable var;

  /* First check if in error state - any key clears it */
  if (error_is_active(&calc)) {
    error_clear(&calc);
    return 0;
  }

  /* Handle STO/RCL wait states */
  if (calc.state == STATE_WAIT_STO || calc.state == STATE_WAIT_RCL) {
    if (is_digit_key(key, &digit)) {
      int memIndex = digit - '0';
      state_handle_memory_digit(&calc, memIndex);
      return 0;
    }
    /* Any other key cancels STO/RCL mode */
    state_cancel_sto_rcl(&calc);
    /* Fall through to process key normally */
  }

  /* Handle digit keys */
  if (is_digit_key(key, &digit)) {
    input_append_digit(&calc, digit);
    return 0;
  }

  /* Handle decimal point */
  if (key == KEY_DOT) {
    input_append_decimal(&calc);
    return 0;
  }

  /* Handle negative sign (+/-) */
  if (key == KEY_NEG) {
    input_toggle_negative(&calc);
    return 0;
  }

  /* Handle backspace / delete */
  if (key == KEY_DEL) {
    input_backspace(&calc);
    return 0;
  }

  /* Handle AC (clear all TVM) */
  if (key == KEY_AC) {
    calc_reset_tvm(&calc);
    return 0;
  }

  /* Handle STO key (OPTN on Casio) */
  if (key == KEY_STO) {
    state_handle_sto_key(&calc);
    return 0;
  }

  /* Handle RCL key (VARS on Casio) */
  if (key == KEY_RCL) {
    state_handle_rcl_key(&calc);
    return 0;
  }

  /* Handle F-keys (TVM variables) */
  if (fkey_to_tvm(key, &var)) {
    state_handle_var_key(&calc, var);
    return 0;
  }

  /* Handle F6 (CPT) */
  if (key == KEY_F6) {
    state_handle_cpt_key(&calc);
    return 0;
  }

  /* Handle SHIFT key (toggle 2nd mode) */
  if (key == KEY_SHIFT) {
    calc.is2ndActive = !calc.is2ndActive;
    return 0;
  }

  /* Handle 2ND + F-key combinations */
  if (calc.is2ndActive) {
    switch (key) {
    case KEY_F2: /* 2ND + I/Y = P/Y settings */
      calc.currentScreen = SCREEN_SETTINGS;
      calc.worksheetIndex = 0;
      calc.is2ndActive = 0;
      return 0;
    case KEY_F3: /* 2ND + PV = AMORT */
      calc.currentScreen = SCREEN_AMORT;
      calc.worksheetIndex = 0;
      calc.is2ndActive = 0;
      return 0;
    case KEY_F4: /* 2ND + PMT = BGN/END toggle */
      calc.tvm.mode = (calc.tvm.mode == TVM_END) ? TVM_BEGIN : TVM_END;
      calc.is2ndActive = 0;
      return 0;
    case KEY_F5: /* 2ND + FV = CLR TVM */
      calc_reset_tvm(&calc);
      calc.is2ndActive = 0;
      return 0;
    case KEY_F6: /* 2ND + CPT = QUIT worksheet */
      calc.currentScreen = SCREEN_TVM;
      calc.worksheetIndex = 0;
      calc.is2ndActive = 0;
      return 0;
    default:
      break;
    }

    /* Worksheet access via 2ND + number keys */
    switch (key) {
    case KEY_7: /* 2ND + 7 = CF (Cash Flow) */
      calc.currentScreen = SCREEN_CASH_FLOW;
      calc.worksheetIndex = 0;
      calc.is2ndActive = 0;
      return 0;
    case KEY_8: /* 2ND + 8 = BOND */
      calc.currentScreen = SCREEN_BOND;
      calc.worksheetIndex = 0;
      calc.is2ndActive = 0;
      return 0;
    case KEY_9: /* 2ND + 9 = DEPR */
      calc.currentScreen = SCREEN_DEPRECIATION;
      calc.worksheetIndex = 0;
      calc.is2ndActive = 0;
      return 0;
    case KEY_4: /* 2ND + 4 = STAT */
      calc.currentScreen = SCREEN_STATISTICS;
      calc.worksheetIndex = 0;
      calc.is2ndActive = 0;
      return 0;
    case KEY_5: /* 2ND + 5 = DATE */
      calc.currentScreen = SCREEN_DATE;
      calc.worksheetIndex = 0;
      calc.is2ndActive = 0;
      return 0;
    case KEY_6: /* 2ND + 6 = BRKEVN (Pro only) */
      if (feature_is_available(calc.model, FEATURE_BREAKEVEN)) {
        calc.currentScreen = SCREEN_BREAKEVEN;
        calc.worksheetIndex = 0;
      }
      calc.is2ndActive = 0;
      return 0;
    case KEY_3: /* 2ND + 3 = PROFIT MARGIN (Pro only) */
      if (feature_is_available(calc.model, FEATURE_BREAKEVEN)) {
        calc.currentScreen = SCREEN_PROFIT_MARGIN;
        calc.worksheetIndex = 0;
      }
      calc.is2ndActive = 0;
      return 0;
    default:
      break;
    }

    /* Handle 2ND + CE = CLR Work (clear current worksheet) */
    if (key == KEY_DEL) { /* CE/C key */
      switch (calc.currentScreen) {
      case SCREEN_TVM:
        calc_reset_tvm(&calc);
        break;
      case SCREEN_CASH_FLOW:
        calc_reset_cashflow(&calc);
        break;
      case SCREEN_BOND:
        calc_reset_bond(&calc);
        break;
      case SCREEN_DEPRECIATION:
        calc_reset_depreciation(&calc);
        break;
      case SCREEN_AMORT:
        calc_reset_tvm(&calc); /* Amort uses TVM data */
        break;
      case SCREEN_STATISTICS:
        calc_reset_statistics(&calc);
        break;
      case SCREEN_BREAKEVEN:
        calc_reset_breakeven(&calc);
        break;
      case SCREEN_PROFIT_MARGIN:
        calc_reset_margin(&calc);
        break;
      default:
        break;
      }
      calc.is2ndActive = 0;
      return 0;
    }

    /* Handle 2ND + . = Format (set decimal places) */
    if (key == KEY_DOT) {
      /* Enter format setting mode - next digit sets decimal places */
      calc.state = STATE_WAIT_FORMAT;
      calc.is2ndActive = 0;
      return 0;
    }
  }

  /* Handle Format digit input (after 2ND + .) */
  if (calc.state == STATE_WAIT_FORMAT) {
    if (is_digit_key(key, &digit)) {
      calc_set_format(&calc, digit - '0'); /* 0-9 */
      calc.state = STATE_INPUT;
      return 0;
    } else if (key == KEY_DOT) {
      calc_set_format(&calc, -1); /* Floating */
      calc.state = STATE_INPUT;
      return 0;
    } else {
      calc.state = STATE_INPUT; /* Cancel on other key */
      return 0;
    }
  }

  /* Handle UP/DOWN arrows for worksheet navigation */
  if (key == KEY_UP) {
    if (calc.worksheetIndex > 0) {
      calc.worksheetIndex--;
    }
    return 0;
  }
  if (key == KEY_DOWN) {
    calc.worksheetIndex++;
    return 0;
  }

  /* Handle EXE (same as pressing current variable) */
  if (key == KEY_EXE) {
    /* Could be used for confirmation in worksheets */
    return 0;
  }

  /* Handle EXIT (return to TVM or exit app) */
  if (key == KEY_EXIT) {
    if (calc.currentScreen != SCREEN_TVM) {
      calc.currentScreen = SCREEN_TVM;
      calc.worksheetIndex = 0;
      return 0;
    }
    return 1; /* Exit the app */
  }

  return 0;
}

/* ============================================================
 * Screen Rendering
 * ============================================================ */

__attribute__((unused)) static void render_screen(void) {
  ui_clear();

  /* Draw status bar (includes STO/RCL indicators) */
  ui_draw_status_bar(&calc);

  /* Draw CPT indicator if in compute mode */
  ui_draw_cpt_indicator(calc.state == STATE_COMPUTE);

  /* Draw main display */
  char displayBuffer[32];

  if (calc.state == STATE_ERROR || error_is_active(&calc)) {
    /* TI BA II Plus style: just show "Error" in display area */
    snprintf(displayBuffer, sizeof(displayBuffer), "%s",
             calc.errorMessage[0] ? calc.errorMessage : "Error");
  } else if (calc.state == STATE_WAIT_STO) {
    /* Show STO prompt - waiting for digit 0-9 */
    snprintf(displayBuffer, sizeof(displayBuffer), "STO 0-9");
  } else if (calc.state == STATE_WAIT_RCL) {
    /* Show RCL prompt - waiting for digit 0-9 */
    snprintf(displayBuffer, sizeof(displayBuffer), "RCL 0-9");
  } else if (calc.inputLength > 0) {
    /* Show input buffer with negative sign if needed */
    if (calc.isNegative) {
      snprintf(displayBuffer, sizeof(displayBuffer), "-%s", calc.inputBuffer);
    } else {
      snprintf(displayBuffer, sizeof(displayBuffer), "%s", calc.inputBuffer);
    }
  } else {
    snprintf(displayBuffer, sizeof(displayBuffer), "0");
  }

  ui_draw_main_display(displayBuffer);

  /* Draw F-key menu */
  ui_draw_tvm_menu();

  /* Refresh display */
  ui_refresh();
}

/* ============================================================
 * Main Entry Point
 * ============================================================ */

#if defined(USE_FXSDK) || defined(USE_CASIO_SDK)
/* Shared calculator loop for device builds */
static int run_calculator_loop(void) {
  int running = 1;
  unsigned long lastTick = hal_system_get_time_ms();

  while (running) {
    state_check_timeout(&calc);
    render_screen();

#ifdef USE_CASIO_SDK
    HAL_Key key = hal_keyboard_wait_key();
    running = !process_key(key);
    lastTick = hal_system_get_time_ms();
#else
    while (1) {
      HAL_Key key = hal_keyboard_get_key();
      unsigned long now = hal_system_get_time_ms();

      if (key != HAL_KEY_NONE) {
        running = !process_key(key);
        lastTick = now;
        break;
      }

      if (now - lastTick >= 100) {
        state_check_timeout(&calc);
        lastTick = now;
        if (calc.state == STATE_INPUT) {
          render_screen();
        }
      }

      hal_system_sleep(10);
    }
#endif
  }

  return 0;
}
#endif

#ifdef USE_FXSDK
/* fxSDK entry point */
int main(void) {
  hal_system_init();
  calc_init(&calc, MODEL_STANDARD);
  ui_init();
  run_calculator_loop();
  hal_system_shutdown();
  return 0;
}
#elif defined(USE_CASIO_SDK)
/* Casio SDK entry point */
int AddIn_main(int isAppli, unsigned short *option) {
  (void)isAppli;
  (void)option;

  hal_system_init();
  calc_init(&calc, MODEL_STANDARD);
  ui_init();
  run_calculator_loop();
  hal_system_shutdown();

  return 1;
}
#else
/* Development/testing entry point (non-SDK) */
int main(int argc, char *argv[]) {
  /* Check for test mode */
  if (argc > 1 && strcmp(argv[1], "--test") == 0) {
    printf("\n🧪 Running CFA Calculator Validation Tests...\n");

    TestSuite suite;
    tests_run_all(&suite);
    tests_print_results(&suite);

    return (suite.failed == 0) ? 0 : 1;
  }

  /* Initialize calculator state */
  calc_init(&calc, MODEL_STANDARD);

  /* Interactive demo mode */
  printf("\n");
  printf("╔══════════════════════════════════════════════════════════════╗\n");
  printf("║           Open fx-BA: TI BA II Plus Clone                    ║\n");
  printf("║                  Development Mode                            ║\n");
  printf("╠══════════════════════════════════════════════════════════════╣\n");
  printf("║  Run with --test flag to execute CFA validation tests       ║\n");
  printf("║  Example: ./fx-ba-test --test                                ║\n");
  printf(
      "╚══════════════════════════════════════════════════════════════╝\n\n");

  /* Demo: TVM Mortgage calculation */
  printf("═══ Demo: Q1 Mortgage Payment ═══\n");
  calc.tvm.N = 360;     /* 30 years * 12 months */
  calc.tvm.I_Y = 5.4;   /* 5.4% annual rate */
  calc.tvm.PV = 250000; /* $250,000 loan */
  calc.tvm.FV = 0;      /* Fully amortizing */
  calc.tvm.P_Y = 12;
  calc.tvm.C_Y = 12;

  double pmt = tvm_solve_for(&calc, TVM_VAR_PMT);

  printf("  Inputs:\n");
  printf("    N   = %.0f (30 years × 12)\n", calc.tvm.N);
  printf("    I/Y = %.2f%%\n", calc.tvm.I_Y);
  printf("    PV  = $%.2f\n", calc.tvm.PV);
  printf("    FV  = $%.2f\n", calc.tvm.FV);
  printf("  Result:\n");
  printf("    PMT = $%.2f\n", pmt);
  printf("  Expected: $-1,403.27\n\n");

  /* Demo: NPV/IRR calculation */
  printf("═══ Demo: Q4/Q5 NPV & IRR ═══\n");
  CashFlowList cf;
  cf_init(&cf);
  cf_set_cf0(&cf, -50000);
  cf_add(&cf, 12000, 1);
  cf_add(&cf, 15000, 1);
  cf_add(&cf, 18000, 1);
  cf_add(&cf, 20000, 1);
  cf_add(&cf, 22000, 1);

  double npv = cf_npv(&cf, 0.10);
  int errCode;
  double irr = cf_irr(&cf, &errCode);

  printf("  Cash Flows:\n");
  printf("    CF0 = $%.0f\n", cf.CF0);
  printf("    C01 = $12,000\n");
  printf("    C02 = $15,000\n");
  printf("    C03 = $18,000\n");
  printf("    C04 = $20,000\n");
  printf("    C05 = $22,000\n");
  printf("  Results (I=10%%):\n");
  printf("    NPV = $%.2f (Expected: $15,465.46)\n", npv);
  printf("    IRR = %.2f%% (Expected: 23.57%%)\n\n", irr * 100);

  /* Demo: BGN mode */
  printf("═══ Demo: Q7 Annuity Due (BGN Mode) ═══\n");
  calc_init(&calc, MODEL_STANDARD);
  calc.tvm.N = 180;
  calc.tvm.I_Y = 6.0;
  calc.tvm.PV = 0;
  calc.tvm.PMT = -500;
  calc.tvm.P_Y = 12;
  calc.tvm.C_Y = 12;
  calc.tvm.mode = TVM_BEGIN;

  double fv = tvm_solve_for(&calc, TVM_VAR_FV);
  printf("  Inputs:\n");
  printf("    Mode = BGN (Beginning of period)\n");
  printf("    N    = 180 (15 years × 12)\n");
  printf("    I/Y  = 6%%\n");
  printf("    PMT  = $-500/month\n");
  printf("  Result:\n");
  printf("    FV   = $%.2f\n", fv);
  printf("  Expected: $146,996.22\n\n");

  printf("═══════════════════════════════════════════════════════════════\n");
  printf("Run './fx-ba-test --test' for full validation suite\n\n");

  return 0;
}
#endif
