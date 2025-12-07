/**
 * Open fx-BA: TI BA II Plus Clone
 * ui.c - User interface rendering implementation
 */

#include "ui.h"
#include "config.h"
#include <stdio.h>
#include <string.h>

/*
 * Note: This file uses placeholder function calls.
 * When using fxSDK/gint, replace with actual gint functions:
 * - dclear() for clearing
 * - dtext() for text rendering
 * - dupdate() for screen refresh
 */

/* Placeholder for Casio display functions */
#ifdef USE_FXSDK
#include <gint/display.h>
#include <gint/keyboard.h>
#else
/* Stub functions for development/testing */
void Bdisp_AllClr_VRAM(void) {}
void Bdisp_PutDisp_DD(void) {}
void PrintMini(int x, int y, const char *str, int mode) {
  (void)x;
  (void)y;
  (void)str;
  (void)mode;
}
#define MINI_OVER 0
#define MINI_REV 1
#endif

/* ============================================================
 * Screen Initialization
 * ============================================================ */
void ui_init(void) { ui_clear(); }

void ui_clear(void) { Bdisp_AllClr_VRAM(); }

void ui_refresh(void) { Bdisp_PutDisp_DD(); }

/* ============================================================
 * Status Bar
 * ============================================================ */
void ui_draw_status_bar(Calculator *calc) {
  int x_pos = 0;

  /* 2ND indicator (if active) */
  if (calc->is2ndActive) {
    PrintMini(x_pos, STATUS_BAR_Y, "2ND", MINI_REV);
    x_pos += 18;
  }

  /* CPT indicator (if in compute mode) */
  if (calc->state == STATE_COMPUTE || calc->isComputeActive) {
    PrintMini(x_pos, STATUS_BAR_Y, "CPT", MINI_REV);
    x_pos += 18;
  }

  /* STO/RCL indicators */
  if (calc->state == STATE_WAIT_STO) {
    PrintMini(x_pos, STATUS_BAR_Y, "STO>", MINI_REV);
    x_pos += 22;
  } else if (calc->state == STATE_WAIT_RCL) {
    PrintMini(x_pos, STATUS_BAR_Y, "RCL>", MINI_REV);
    x_pos += 22;
  }

  /* TVM mode indicator (BGN/END) */
  const char *modeStr = (calc->tvm.mode == TVM_BEGIN) ? "BGN" : "";
  if (calc->tvm.mode == TVM_BEGIN) {
    PrintMini(x_pos, STATUS_BAR_Y, modeStr, MINI_OVER);
  }

  /* Model indicator at right side */
  const char *modelStr = (calc->model == MODEL_PROFESSIONAL) ? "PRO" : "";
  if (calc->model == MODEL_PROFESSIONAL) {
    PrintMini(SCREEN_WIDTH - 20, STATUS_BAR_Y, modelStr, MINI_OVER);
  }
}

/* STO indicator helper */
void ui_draw_sto_indicator(int active) {
  if (active) {
    PrintMini(0, STATUS_BAR_Y, "STO>", MINI_REV);
  }
}

/* RCL indicator helper */
void ui_draw_rcl_indicator(int active) {
  if (active) {
    PrintMini(0, STATUS_BAR_Y, "RCL>", MINI_REV);
  }
}

/* 2ND indicator helper */
void ui_draw_2nd_indicator(int active) {
  if (active) {
    PrintMini(0, STATUS_BAR_Y, "2ND", MINI_REV);
  }
}

/* ============================================================
 * Main Display
 * ============================================================ */
void ui_draw_main_display(const char *text) {
  /* Draw large, right-aligned number in center of screen */
  ui_draw_right_aligned(MAIN_DISPLAY_Y, text);
}

void ui_draw_right_aligned(int y, const char *text) {
  int len = strlen(text);
  int x = SCREEN_WIDTH - (len * CHAR_WIDTH) - 4; /* 4px padding from right */

  if (x < 0)
    x = 0;

  PrintMini(x, y, text, MINI_OVER);
}

/**
 * Draw worksheet-style display with label on left, value on right.
 * e.g., "CF0 =" on left, "-1,000.00" on right
 */
void ui_draw_display_with_label(const char *label, const char *value) {
  /* Draw label on left side */
  PrintMini(2, MAIN_DISPLAY_Y - 8, label, MINI_OVER);
  
  /* Draw value right-aligned below */
  ui_draw_right_aligned(MAIN_DISPLAY_Y + 4, value);
}

/* ============================================================
 * F-Key Menu
 * ============================================================ */
void ui_draw_fkey_menu(const char *labels[], int count) {
  if (count > 6)
    count = 6;

  int spacing = SCREEN_WIDTH / 6;

  for (int i = 0; i < count; i++) {
    int x = i * spacing + 2;
    PrintMini(x, FKEY_MENU_Y, labels[i], MINI_OVER);
  }
}

void ui_draw_tvm_menu(void) {
  static const char *tvm_labels[] = {"N", "I/Y", "PV", "PMT", "FV", "CPT"};
  ui_draw_fkey_menu(tvm_labels, 6);
}

void ui_draw_cpt_indicator(int active) {
  if (active) {
    PrintMini(0, MAIN_DISPLAY_Y - 10, "COMPUTE", MINI_REV);
  }
}

/**
 * Draw up/down arrow hints for worksheet navigation.
 * Shows small arrows to indicate scrollable content.
 */
void ui_draw_worksheet_hints(int showUp, int showDown) {
  if (showUp) {
    PrintMini(SCREEN_WIDTH - 10, MAIN_DISPLAY_Y - 12, "^", MINI_OVER);
  }
  if (showDown) {
    PrintMini(SCREEN_WIDTH - 10, MAIN_DISPLAY_Y + 16, "v", MINI_OVER);
  }
}

/* ============================================================
 * Error Display (TI BA II Plus style)
 * 
 * Shows "Error" in the main display area without clearing
 * the calculator state. User presses any key to clear.
 * ============================================================ */
void ui_show_error(int errorCode) {
  /* Display "Error" in the main number area */
  ui_draw_right_aligned(MAIN_DISPLAY_Y, "Error");
  ui_refresh();
}

/**
 * Show error message inline (in the main display area).
 * This is the TI BA II Plus style - error replaces number display.
 */
void ui_show_error_inline(const char *message) {
  /* Just show the error message right-aligned like a number */
  ui_draw_right_aligned(MAIN_DISPLAY_Y, message);
}

/* ============================================================
 * Memory Feedback Display
 * ============================================================ */

/**
 * Brief feedback when storing to memory.
 * Shows "M#=value" format for user confirmation.
 */
void ui_show_memory_stored(int index, double value) {
  char buf[24];
  snprintf(buf, sizeof(buf), "M%d=", index);
  PrintMini(2, MAIN_DISPLAY_Y - 8, buf, MINI_OVER);
  
  char valBuf[16];
  format_number(value, valBuf, sizeof(valBuf));
  ui_draw_right_aligned(MAIN_DISPLAY_Y + 4, valBuf);
}

/**
 * Brief feedback when recalling from memory.
 */
void ui_show_memory_recalled(int index, double value) {
  char buf[24];
  snprintf(buf, sizeof(buf), "RCL M%d", index);
  PrintMini(2, MAIN_DISPLAY_Y - 8, buf, MINI_OVER);
  
  char valBuf[16];
  format_number(value, valBuf, sizeof(valBuf));
  ui_draw_right_aligned(MAIN_DISPLAY_Y + 4, valBuf);
}

/* ============================================================
 * Popup Messages
 * ============================================================ */
void ui_show_message(const char *title, const char *message) {
  ui_clear();
  PrintMini(0, 10, title, MINI_REV);
  PrintMini(0, 25, message, MINI_OVER);
  ui_refresh();
}

/* ============================================================
 * Number Formatting
 * ============================================================ */
void format_number(double value, char *buffer, int maxLen) {
  /* Format with appropriate precision */
  /* TI shows up to 10 significant digits */

  if (value == 0.0) {
    snprintf(buffer, maxLen, "0");
    return;
  }

  /* Handle large/small numbers with scientific notation */
  double absVal = value < 0 ? -value : value;

  if (absVal >= 1e10 || absVal < 1e-9) {
    snprintf(buffer, maxLen, "%.4e", value);
  } else {
    /* Regular decimal format, remove trailing zeros */
    snprintf(buffer, maxLen, "%.9f", value);

    /* Remove trailing zeros */
    int len = strlen(buffer);
    while (len > 1 && buffer[len - 1] == '0' && buffer[len - 2] != '.') {
      buffer[--len] = '\0';
    }
  }
}

void format_with_commas(double value, char *buffer, int maxLen) {
  /* For now, just use regular format */
  /* TODO: Add comma separators for large numbers */
  format_number(value, buffer, maxLen);
}
