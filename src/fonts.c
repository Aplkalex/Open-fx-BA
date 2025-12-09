/**
 * Open fx-BA: TI BA II Plus Clone
 * fonts.c - Large font rendering for authentic calculator look
 *
 * Implements:
 * - 7-segment style large digits (8x12 pixels)
 * - Medium font for F-key labels
 * - Status bar icons
 */

#include "fonts.h"
#include "config.h"
#include "hal/hal_display.h"
#include <string.h>

/* Wrapper macros to use HAL functions with SDK-style API */
#define MINI_OVER 0
#define MINI_REV 1
#define PrintMini(x, y, str, mode)                                             \
  hal_display_print_text((x), (y), (str),                                      \
                         (mode) == MINI_REV ? HAL_TEXT_REVERSE                 \
                                            : HAL_TEXT_NORMAL)
#define Bdisp_SetPoint_VRAM(x, y, c) hal_display_set_pixel((x), (y), (c))
#define Bdisp_AreaReverseVRAM(x1, y1, x2, y2)                                  \
  hal_display_area_reverse((x1), (y1), (x2), (y2))

/* ============================================================
 * 7-Segment Digit Patterns (8x12 pixels)
 *
 * Layout:
 *   aaaa
 *  f    b
 *  f    b
 *   gggg
 *  e    c
 *  e    c
 *   dddd
 * ============================================================ */

/* Segment bitmasks: a=0x01, b=0x02, c=0x04, d=0x08, e=0x10, f=0x20, g=0x40 */
static const unsigned char DIGIT_SEGMENTS[10] = {
    0x3F, /* 0: a,b,c,d,e,f */
    0x06, /* 1: b,c */
    0x5B, /* 2: a,b,d,e,g */
    0x4F, /* 3: a,b,c,d,g */
    0x66, /* 4: b,c,f,g */
    0x6D, /* 5: a,c,d,f,g */
    0x7D, /* 6: a,c,d,e,f,g */
    0x07, /* 7: a,b,c */
    0x7F, /* 8: all */
    0x6F  /* 9: a,b,c,d,f,g */
};

/* Draw a horizontal segment (for a, d, g) */
static void draw_h_segment(int x, int y, int width) {
  for (int i = 1; i < width - 1; i++) {
    Bdisp_SetPoint_VRAM(x + i, y, 1);
  }
}

/* Draw a vertical segment (for b, c, e, f) */
static void draw_v_segment(int x, int y, int height) {
  for (int i = 1; i < height - 1; i++) {
    Bdisp_SetPoint_VRAM(x, y + i, 1);
  }
}

void font_draw_digit(int x, int y, char digit) {
  if (digit < '0' || digit > '9')
    return;

  unsigned char seg = DIGIT_SEGMENTS[digit - '0'];
  int w = LARGE_CHAR_WIDTH - 2;        /* Segment width */
  int h = (LARGE_CHAR_HEIGHT / 2) - 1; /* Half height for segments */

  /* Segment a (top) */
  if (seg & 0x01)
    draw_h_segment(x, y, w);

  /* Segment b (top-right) */
  if (seg & 0x02)
    draw_v_segment(x + w - 1, y, h);

  /* Segment c (bottom-right) */
  if (seg & 0x04)
    draw_v_segment(x + w - 1, y + h, h);

  /* Segment d (bottom) */
  if (seg & 0x08)
    draw_h_segment(x, y + LARGE_CHAR_HEIGHT - 2, w);

  /* Segment e (bottom-left) */
  if (seg & 0x10)
    draw_v_segment(x, y + h, h);

  /* Segment f (top-left) */
  if (seg & 0x20)
    draw_v_segment(x, y, h);

  /* Segment g (middle) */
  if (seg & 0x40)
    draw_h_segment(x, y + h, w);
}

void font_draw_decimal(int x, int y) {
  /* Draw a 2x2 dot at bottom */
  int dotY = y + LARGE_CHAR_HEIGHT - 3;
  Bdisp_SetPoint_VRAM(x, dotY, 1);
  Bdisp_SetPoint_VRAM(x + 1, dotY, 1);
  Bdisp_SetPoint_VRAM(x, dotY + 1, 1);
  Bdisp_SetPoint_VRAM(x + 1, dotY + 1, 1);
}

void font_draw_minus(int x, int y) {
  /* Draw horizontal line in middle */
  int midY = y + LARGE_CHAR_HEIGHT / 2;
  for (int i = 0; i < LARGE_CHAR_WIDTH - 2; i++) {
    Bdisp_SetPoint_VRAM(x + i, midY, 1);
  }
}

/* ============================================================
 * Number String Rendering
 * ============================================================ */

int font_get_width(const char *str) {
  int width = 0;
  for (int i = 0; str[i]; i++) {
    if (str[i] == '.') {
      width += 3; /* Decimal point is narrow */
    } else if (str[i] == ',') {
      width += 3; /* Comma is narrow */
    } else if (str[i] == '-') {
      width += LARGE_CHAR_WIDTH;
    } else {
      width += LARGE_CHAR_WIDTH;
    }
  }
  return width;
}

void font_draw_number(int rightX, int y, const char *str) {
  int totalWidth = font_get_width(str);
  int x = rightX - totalWidth;

  for (int i = 0; str[i]; i++) {
    char c = str[i];

    if (c >= '0' && c <= '9') {
      font_draw_digit(x, y, c);
      x += LARGE_CHAR_WIDTH;
    } else if (c == '.') {
      font_draw_decimal(x, y);
      x += 3;
    } else if (c == ',') {
      /* Draw comma as small mark */
      Bdisp_SetPoint_VRAM(x, y + LARGE_CHAR_HEIGHT - 4, 1);
      Bdisp_SetPoint_VRAM(x, y + LARGE_CHAR_HEIGHT - 3, 1);
      x += 3;
    } else if (c == '-') {
      font_draw_minus(x, y);
      x += LARGE_CHAR_WIDTH;
    }
  }
}

/* ============================================================
 * Medium Font (uses PrintMini but with better spacing)
 * ============================================================ */

void font_draw_medium(int x, int y, const char *str) {
  PrintMini(x, y, str, MINI_OVER);
}

/* ============================================================
 * F-Key Labels with Box Background
 * ============================================================ */

void font_draw_fkey_label(int index, const char *label, int highlighted) {
  int spacing = SCREEN_WIDTH / 6;
  int x = index * spacing;
  int y = 56; /* Bottom of screen */
  int boxWidth = spacing - 2;

  if (highlighted) {
    /* Draw reverse video box */
    Bdisp_AreaReverseVRAM(x, y, x + boxWidth, y + 7);
  }

  /* Center the label in the box */
  int labelLen = strlen(label);
  int textX = x + (boxWidth - labelLen * 4) / 2;

  PrintMini(textX, y, label, highlighted ? MINI_REV : MINI_OVER);
}

/* ============================================================
 * Status Bar Icons
 * ============================================================ */

void icon_draw_2nd(int x, int y, int active) {
  if (active) {
    /* Draw small box with "2" in reverse */
    Bdisp_AreaReverseVRAM(x, y, x + 11, y + 6);
    PrintMini(x + 1, y, "2nd", MINI_REV);
  }
}

void icon_draw_cpt(int x, int y, int blinking) {
  if (blinking) {
    PrintMini(x, y, "CPT", MINI_REV);
  } else {
    PrintMini(x, y, "CPT", MINI_OVER);
  }
}

void icon_draw_bgn(int x, int y, int active) {
  if (active) {
    PrintMini(x, y, "BGN", MINI_OVER);
  }
}

void icon_draw_memory(int x, int y, int memIndex) {
  char buf[4];
  buf[0] = 'M';
  buf[1] = '0' + memIndex;
  buf[2] = '\0';
  PrintMini(x, y, buf, MINI_OVER);
}
