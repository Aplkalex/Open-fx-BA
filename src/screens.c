/**
 * Open fx-BA: TI BA II Plus Clone
 * screens.c - TI-style screen implementation
 *
 * Creates authentic TI BA II Plus look and feel:
 * - Single-line calculator display (right-aligned)
 * - F-key menu system
 * - 2nd function indicators
 * - Worksheet navigation (up/down arrows)
 */

#include "screens.h"
#include "config.h"
#include "hal/hal_display.h"
#include "ui.h"
#include <stdio.h>
#include <string.h>

/* Wrapper macros to use HAL functions with PrintMini-style API */
#define MINI_OVER 0
#define MINI_REV 1
#define PrintMini(x, y, str, mode)                                             \
  hal_display_print_text((x), (y), (str),                                      \
                         (mode) == MINI_REV ? HAL_TEXT_REVERSE                 \
                                            : HAL_TEXT_NORMAL)

/* ============================================================
 * Static Menu Definitions
 * ============================================================ */

/* TVM Main Menu */
static FKeyMenu tvmMenu = {.items = {{"N", "xP/Y", TVM_VAR_N, 0},
                                     {"I/Y", "P/Y", TVM_VAR_IY, 0},
                                     {"PV", "AMORT", TVM_VAR_PV, 0},
                                     {"PMT", "BGN", TVM_VAR_PMT, 0},
                                     {"FV", "CLR", TVM_VAR_FV, 0},
                                     {"CPT", "QUIT", -1, 0}},
                           .count = 6,
                           .hasPage2 = 0};

/* Cash Flow Menu */
static FKeyMenu cashflowMenu = {
    .items = {{"CF0", "", 0, 0},
              {"C01", "F01", 1, 0},
              {"NPV", "NFV", 2, 0},
              {"IRR", "MIRR", 3, 0},
              {"↑↓", "INS", 4, 0},
              {"CPT", "CLR", 5, 0}},
    .count = 6,
    .hasPage2 = 1 /* PB, DPB on page 2 for Pro */
};

/* Bond Menu */
static FKeyMenu bondMenu = {
    .items =
        {
            {"SDT", "", 0, 0},    /* Settlement date */
            {"CPN", "", 1, 0},    /* Coupon rate */
            {"RDT", "", 2, 0},    /* Redemption date */
            {"PRI", "YLD", 3, 0}, /* Price / Yield */
            {"AI", "DUR", 4, 0},  /* Accrued Int / Duration */
            {"CPT", "SET", 5, 0}  /* Compute / Settings */
        },
    .count = 6,
    .hasPage2 = 0};

/* Depreciation Menu */
static FKeyMenu depreciationMenu = {
    .items =
        {
            {"LIF", "", 0, 0},   /* Life */
            {"MON", "", 1, 0},   /* Start month */
            {"CST", "", 2, 0},   /* Cost */
            {"SAL", "", 3, 0},   /* Salvage */
            {"YR", "DEP", 4, 0}, /* Year / Depreciation */
            {"SET", "", 5, 0}    /* Method selection */
        },
    .count = 6,
    .hasPage2 = 0};

/* ============================================================
 * Menu Getters
 * ============================================================ */

FKeyMenu *get_tvm_menu(void) { return &tvmMenu; }
FKeyMenu *get_cashflow_menu(void) { return &cashflowMenu; }
FKeyMenu *get_bond_menu(void) { return &bondMenu; }
FKeyMenu *get_depreciation_menu(void) { return &depreciationMenu; }

/* ============================================================
 * Screen Initialization
 * ============================================================ */

void screen_init(ScreenState *state) {
  state->currentScreen = SCREEN_TVM;
  state->menuPage = 0;
  state->selectedEntry = 0;
  state->is2ndActive = 0;
  state->isCPTActive = 0;
}

/* ============================================================
 * Main Screen Drawing
 * ============================================================ */

void screen_draw(Calculator *calc, ScreenState *state) {
  ui_clear();

  switch (state->currentScreen) {
  case SCREEN_TVM:
    screen_draw_tvm(calc, state);
    break;
  case SCREEN_CASH_FLOW:
    screen_draw_worksheet(calc, state, NULL, 0);
    break;
  case SCREEN_BOND:
    screen_draw_worksheet(calc, state, NULL, 0);
    break;
  case SCREEN_DEPRECIATION:
    screen_draw_worksheet(calc, state, NULL, 0);
    break;
  case SCREEN_STATISTICS:
    screen_draw_worksheet(calc, state, NULL, 0);
    break;
  case SCREEN_DATE:
    screen_draw_worksheet(calc, state, NULL, 0);
    break;
  case SCREEN_AMORT:
    screen_draw_worksheet(calc, state, NULL, 0);
    break;
  default:
    screen_draw_tvm(calc, state);
    break;
  }

  ui_refresh();
}

/* ============================================================
 * TVM Screen (Main Calculator Display)
 * ============================================================ */

void screen_draw_tvm(Calculator *calc, ScreenState *state) {
  char buffer[32];

  /* ---- Status Bar (Line 0) ---- */
  /* Left side: Model indicator */
  const char *modelStr = (calc->model == MODEL_PROFESSIONAL) ? "PRO" : "STD";
  PrintMini(0, 0, modelStr, MINI_OVER);

  /* Center: BGN/END indicator */
  const char *modeStr = (calc->tvm.mode == TVM_BEGIN) ? "BGN" : "";
  PrintMini(40, 0, modeStr, MINI_OVER);

  /* Right side: 2nd indicator */
  if (state->is2ndActive) {
    PrintMini(100, 0, "2nd", MINI_REV); /* Reverse video */
  }

  /* ---- Horizontal Separator ---- */
  /* Draw a line at y=8 */
  for (int x = 0; x < SCREEN_WIDTH; x += 2) {
    PrintMini(x, 8, "-", MINI_OVER);
  }

  /* ---- CPT Indicator ---- */
  if (state->isCPTActive) {
    PrintMini(0, 12, "COMPUTE", MINI_REV);
  }

  /* ---- Main Display (Large Right-Aligned Number) ---- */
  /* This is the main input/output area */
  if (calc->state == STATE_ERROR) {
    snprintf(buffer, sizeof(buffer), "Error %d", calc->errorCode);
  } else if (calc->inputLength > 0) {
    if (calc->isNegative) {
      snprintf(buffer, sizeof(buffer), "-%s", calc->inputBuffer);
    } else {
      snprintf(buffer, sizeof(buffer), "%s", calc->inputBuffer);
    }
  } else {
    snprintf(buffer, sizeof(buffer), "0");
  }

  /* Draw right-aligned at y=28 (center of screen) */
  int textLen = strlen(buffer);
  int x = SCREEN_WIDTH - (textLen * 6) - 4; /* Larger font = 6px wide */
  if (x < 4)
    x = 4;

  /* For larger display, we could use PrintXY or custom large font */
  /* For now, using PrintMini at a readable position */
  PrintMini(x, 28, buffer, MINI_OVER);

  /* ---- Variable Indicator (show which TVM var was last accessed) ---- */
  /* Small indicator showing last variable */

  /* ---- F-Key Menu (Bottom) ---- */
  draw_fkey_menu(&tvmMenu, state->is2ndActive);
}

/* ============================================================
 * Worksheet Style Screen
 * ============================================================ */

void screen_draw_worksheet(Calculator *calc, ScreenState *state,
                           WorksheetEntry entries[], int count) {
  (void)calc; /* Unused for now */

  char buffer[32];
  int startY = 10;
  int lineHeight = 10;

  /* Draw up to 4 entries visible at once */
  int startIdx = state->selectedEntry > 2 ? state->selectedEntry - 2 : 0;
  int endIdx = startIdx + 4;
  if (endIdx > count)
    endIdx = count;

  for (int i = startIdx; i < endIdx; i++) {
    int y = startY + (i - startIdx) * lineHeight;
    int isSelected = (i == state->selectedEntry);

    /* Label */
    PrintMini(4, y, entries[i].label, isSelected ? MINI_REV : MINI_OVER);

    /* Value */
    if (entries[i].valuePtr) {
      format_number(*entries[i].valuePtr, buffer, sizeof(buffer));
    } else {
      snprintf(buffer, sizeof(buffer), "0");
    }

    /* Right-align the value */
    int valueLen = strlen(buffer);
    int valueX = SCREEN_WIDTH - (valueLen * CHAR_WIDTH) - 4;
    PrintMini(valueX, y, buffer, isSelected ? MINI_REV : MINI_OVER);

    /* Computed indicator */
    if (entries[i].computed) {
      PrintMini(SCREEN_WIDTH - 12, y, "*", MINI_OVER);
    }
  }

  /* Scroll indicators */
  if (startIdx > 0) {
    PrintMini(SCREEN_WIDTH / 2, 2, "▲", MINI_OVER);
  }
  if (endIdx < count) {
    PrintMini(SCREEN_WIDTH / 2, 50, "▼", MINI_OVER);
  }
}

/* ============================================================
 * F-Key Menu Drawing
 * ============================================================ */

void draw_fkey_menu(FKeyMenu *menu, int is2ndActive) {
  int y = FKEY_MENU_Y;
  int spacing = SCREEN_WIDTH / 6;

  /* Draw separator line */
  for (int x = 0; x < SCREEN_WIDTH; x += 2) {
    PrintMini(x, y - 4, "-", MINI_OVER);
  }

  /* Draw each F-key label */
  for (int i = 0; i < menu->count && i < 6; i++) {
    int x = (i * spacing) + (spacing / 2) - 8;
    if (x < 0)
      x = 0;

    const char *label;

    /* Show 2nd function label if 2nd is active and available */
    if (is2ndActive && menu->items[i].label2nd[0] != '\0') {
      label = menu->items[i].label2nd;
      /* Draw in reverse video to indicate 2nd function */
      PrintMini(x, y, label, MINI_REV);
    } else {
      label = menu->items[i].label;
      PrintMini(x, y, label, MINI_OVER);
    }
  }

  /* If there's a page 2, show indicator */
  if (menu->hasPage2) {
    PrintMini(SCREEN_WIDTH - 8, y - 10, ">", MINI_OVER);
  }
}
