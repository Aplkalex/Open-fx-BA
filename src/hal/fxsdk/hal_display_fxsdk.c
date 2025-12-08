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
  color_t fg = C_BLACK;
  color_t bg = C_WHITE;

  if (mode == HAL_TEXT_REVERSE) {
    fg = C_WHITE;
    bg = C_BLACK;
  }

  dtext(x, y, fg, text);
}

int hal_display_get_width(void) { return 128; /* fx-9860G width */ }

int hal_display_get_height(void) { return 64; /* fx-9860G height */ }

#else

/* Stub implementations for non-fxSDK builds */
#include <stdio.h>

void hal_display_clear(void) { printf("[HAL] Display cleared\n"); }

void hal_display_refresh(void) { printf("[HAL] Display refreshed\n"); }

void hal_display_print_text(int x, int y, const char *text, HAL_TextMode mode) {
  printf("[HAL] Print at (%d,%d): %s (mode=%d)\n", x, y, text, mode);
}

int hal_display_get_width(void) { return 128; }

int hal_display_get_height(void) { return 64; }

#endif /* USE_FXSDK */
