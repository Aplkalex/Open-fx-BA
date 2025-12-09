/**
 * Open fx-BA: TI BA II Plus Clone
 * ui.c - User interface rendering implementation
 */

#include "ui.h"
#include "config.h"
#include "hal/hal_display.h"
#include <stdio.h>
#include <string.h>

static void draw_text(int x, int y, const char *text, int reverse) {
  hal_display_print_text(x, y, text,
                         reverse ? HAL_TEXT_REVERSE : HAL_TEXT_NORMAL);
}

/* ============================================================
 * Screen Initialization
 * ============================================================ */
void ui_init(void) { ui_clear(); }

void ui_clear(void) { hal_display_clear(); }

void ui_refresh(void) { hal_display_refresh(); }

/* ============================================================
 * Status Bar
 * ============================================================ */
void ui_draw_status_bar(Calculator *calc) {
  int x_pos = 0;

  /* 2ND indicator (if active) */
  if (calc->is2ndActive) {
    draw_text(x_pos, STATUS_BAR_Y, "2ND", 1);
    x_pos += 18;
  }

  /* CPT indicator (if in compute mode) */
  if (calc->state == STATE_COMPUTE || calc->isComputeActive) {
    draw_text(x_pos, STATUS_BAR_Y, "CPT", 1);
    x_pos += 18;
  }

  /* STO/RCL indicators */
  if (calc->state == STATE_WAIT_STO) {
    draw_text(x_pos, STATUS_BAR_Y, "STO>", 1);
    x_pos += 22;
  } else if (calc->state == STATE_WAIT_RCL) {
    draw_text(x_pos, STATUS_BAR_Y, "RCL>", 1);
    x_pos += 22;
  }

  /* TVM mode indicator (BGN/END) */
  const char *modeStr = (calc->tvm.mode == TVM_BEGIN) ? "BGN" : "";
  if (calc->tvm.mode == TVM_BEGIN) {
    draw_text(x_pos, STATUS_BAR_Y, modeStr, 0);
  }

  /* Model indicator at right side */
  const char *modelStr = (calc->model == MODEL_PROFESSIONAL) ? "PRO" : "STD";
  draw_text(SCREEN_WIDTH - 20, STATUS_BAR_Y, modelStr, 0);
}

/* STO indicator helper */
void ui_draw_sto_indicator(int active) {
  if (active) {
    draw_text(0, STATUS_BAR_Y, "STO>", 1);
  }
}

/* RCL indicator helper */
void ui_draw_rcl_indicator(int active) {
  if (active) {
    draw_text(0, STATUS_BAR_Y, "RCL>", 1);
  }
}

/* 2ND indicator helper */
void ui_draw_2nd_indicator(int active) {
  if (active) {
    draw_text(0, STATUS_BAR_Y, "2ND", 1);
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

  draw_text(x, y, text, 0);
}

/**
 * Draw worksheet-style display with label on left, value on right.
 * e.g., "CF0 =" on left, "-1,000.00" on right
 */
void ui_draw_display_with_label(const char *label, const char *value) {
  /* Draw label on left side */
  draw_text(2, MAIN_DISPLAY_Y - 8, label, 0);

  /* Draw value right-aligned below */
  ui_draw_right_aligned(MAIN_DISPLAY_Y + 4, value);
}

/* ============================================================
 * F-Key Menu
 * ============================================================ */
void ui_draw_fkey_menu(const char *labels[], int count, int reverse) {
  (void)reverse; /* Reverse not needed with boxed labels, keep signature for API */
  if (count > 6)
    count = 6;

  int spacing = SCREEN_WIDTH / 6;

  for (int i = 0; i < count; i++) {
    int x = i * spacing + 2;
    if (labels[i] == NULL || labels[i][0] == '\0')
      continue;

    /* Draw a small reversed capsule behind the label to mimic TI-style keys */
    int textLen = strlen(labels[i]);
    int boxWidth = (textLen * CHAR_WIDTH) + 6;
    int boxX1 = x - 2;
    int boxY1 = FKEY_MENU_Y - 2;
    int boxX2 = boxX1 + boxWidth;
    int boxY2 = boxY1 + CHAR_HEIGHT + 2;

    if (boxX1 < 0)
      boxX1 = 0;
    if (boxX2 > SCREEN_WIDTH)
      boxX2 = SCREEN_WIDTH;

    hal_display_area_reverse(boxX1, boxY1, boxX2, boxY2);
    draw_text(x, FKEY_MENU_Y, labels[i], 1);
  }
}

void ui_draw_tvm_menu(int isSecond) {
  static const char *tvm_labels[] = {"N",    "I/Y",  "PV",
                                     "PMT", "FV",   "CPT"};
  static const char *tvm_labels_2nd[] = {"xP/Y", "P/Y",   "AMORT",
                                         "BGN",  "CLR",  "QUIT"};

  const char **labels = isSecond ? tvm_labels_2nd : tvm_labels;
  ui_draw_fkey_menu(labels, 6, isSecond);
}

void ui_draw_cpt_indicator(int active) {
  if (active) {
    draw_text(0, MAIN_DISPLAY_Y - 10, "COMPUTE", 1);
  }
}

/**
 * Draw up/down arrow hints for worksheet navigation.
 * Shows small arrows to indicate scrollable content.
 */
void ui_draw_worksheet_hints(int showUp, int showDown) {
  if (showUp) {
    draw_text(SCREEN_WIDTH - 10, MAIN_DISPLAY_Y - 12, "^", 0);
  }
  if (showDown) {
    draw_text(SCREEN_WIDTH - 10, MAIN_DISPLAY_Y + 16, "v", 0);
  }
}

/* ============================================================
 * Error Display (TI BA II Plus style)
 *
 * Shows "Error" in the main display area without clearing
 * the calculator state. User presses any key to clear.
 * ============================================================ */
void ui_show_error(int errorCode) {
  /* Display error message based on code */
  const char *msg;
  switch (errorCode) {
  case 1:
    msg = "No Solution";
    break;
  case 2:
    msg = "Overflow";
    break;
  case 3:
    msg = "No Converge";
    break;
  case 4:
    msg = "Bad Input";
    break;
  case 5:
    msg = "Multi IRR";
    break;
  default:
    msg = "Error";
    break;
  }
  ui_draw_right_aligned(MAIN_DISPLAY_Y, msg);
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
  draw_text(2, MAIN_DISPLAY_Y - 8, buf, 0);

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
  draw_text(2, MAIN_DISPLAY_Y - 8, buf, 0);

  char valBuf[16];
  format_number(value, valBuf, sizeof(valBuf));
  ui_draw_right_aligned(MAIN_DISPLAY_Y + 4, valBuf);
}

/* ============================================================
 * Popup Messages
 * ============================================================ */
void ui_show_message(const char *title, const char *message) {
  ui_clear();
  draw_text(0, 10, title, 1);
  draw_text(0, 25, message, 0);
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
  /*
   * Format number with thousand separators.
   * Examples: 1234567.89 -> "1,234,567.89"
   *          -50000 -> "-50,000"
   */

  /* First, format without commas */
  char temp[32];
  format_number(value, temp, sizeof(temp));

  /* Handle simple cases */
  int len = strlen(temp);
  if (len >= maxLen || len < 4) {
    strncpy(buffer, temp, maxLen - 1);
    buffer[maxLen - 1] = '\0';
    return;
  }

  /* Find decimal point position (or end of string) */
  char *decimalPos = strchr(temp, '.');
  int intPartLen = decimalPos ? (int)(decimalPos - temp) : len;

  /* Handle negative sign */
  int isNegative = (temp[0] == '-');
  int digitStart = isNegative ? 1 : 0;
  int numDigits = intPartLen - digitStart;

  /* Calculate number of commas needed */
  int numCommas = (numDigits - 1) / 3;

  /* Check if result fits in buffer */
  int resultLen = len + numCommas;
  if (resultLen >= maxLen) {
    strncpy(buffer, temp, maxLen - 1);
    buffer[maxLen - 1] = '\0';
    return;
  }

  /* Build result with commas */
  int srcIdx = 0;
  int dstIdx = 0;

  /* Copy negative sign if present */
  if (isNegative) {
    buffer[dstIdx++] = temp[srcIdx++];
    numDigits--;
  }

  /* Copy integer part with commas */
  int digitCount = 0;
  int firstGroupSize = numDigits % 3;
  if (firstGroupSize == 0)
    firstGroupSize = 3;

  while (srcIdx < intPartLen) {
    buffer[dstIdx++] = temp[srcIdx++];
    digitCount++;

    /* Insert comma after each group of 3, except after last group */
    if (srcIdx < intPartLen) {
      if (digitCount == firstGroupSize ||
          (digitCount > firstGroupSize &&
           (digitCount - firstGroupSize) % 3 == 0)) {
        buffer[dstIdx++] = ',';
      }
    }
  }

  /* Copy decimal part if present */
  if (decimalPos) {
    while (*decimalPos) {
      buffer[dstIdx++] = *decimalPos++;
    }
  }

  buffer[dstIdx] = '\0';
}
