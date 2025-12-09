/**
 * fxSDK Display Adapter
 * Implements HAL display interface using fxSDK/gint
 */

#include "../hal_display.h"

#ifdef USE_FXSDK
#include <gint/display.h>
#include <gint/std/string.h>

void hal_display_clear(void) { dclear(C_WHITE); }

void hal_display_refresh(void) { dupdate(); }

void hal_display_print_text(int x, int y, const char *text, HAL_TextMode mode) {
  if (mode == HAL_TEXT_REVERSE) {
    int width = strlen(text) * 6;
    drectfill(x, y, x + width, y + 7, C_BLACK);
    dtext(x, y, C_WHITE, text);
  } else {
    dtext(x, y, C_BLACK, text);
  }
}

int hal_display_get_width(void) { return 128; /* fx-9860G width */ }

int hal_display_get_height(void) { return 64; /* fx-9860G height */ }

void hal_display_set_pixel(int x, int y, int color) {
  dpixel(x, y, color ? C_BLACK : C_WHITE);
}

void hal_display_area_reverse(int x1, int y1, int x2, int y2) {
  for (int y = y1; y <= y2; y++) {
    for (int x = x1; x <= x2; x++) {
      int current = dpixel(x, y, C_NONE);
      dpixel(x, y, current == C_BLACK ? C_WHITE : C_BLACK);
    }
  }
}

#else

/* Stub implementations for non-fxSDK builds */
#include <stdio.h>

void hal_display_clear(void) { /* stub */ }

void hal_display_refresh(void) { /* stub */ }

void hal_display_print_text(int x, int y, const char *text, HAL_TextMode mode) {
  (void)x;
  (void)y;
  (void)text;
  (void)mode;
}

int hal_display_get_width(void) { return 128; }

int hal_display_get_height(void) { return 64; }

void hal_display_set_pixel(int x, int y, int color) {
  (void)x;
  (void)y;
  (void)color;
}

void hal_display_area_reverse(int x1, int y1, int x2, int y2) {
  (void)x1;
  (void)y1;
  (void)x2;
  (void)y2;
}

#endif /* USE_FXSDK */
