/**
 * Casio SDK Display Adapter
 * Implements HAL display interface using official Casio SDK (fxlib.h)
 */

#include "../hal_display.h"

#ifdef USE_CASIO_SDK
#include <fxlib.h>

void hal_display_clear(void) { Bdisp_AllClr_VRAM(); }

void hal_display_refresh(void) { Bdisp_PutDisp_DD(); }

void hal_display_print_text(int x, int y, const char *text, HAL_TextMode mode) {
  int print_mode;

  switch (mode) {
  case HAL_TEXT_REVERSE:
    print_mode = TEXT_MODE_REVERSE;
    break;
  case HAL_TEXT_TRANSPARENT:
    print_mode = TEXT_MODE_TRANSPARENT_BACKGROUND;
    break;
  case HAL_TEXT_NORMAL:
  default:
    print_mode = TEXT_MODE_NORMAL;
    break;
  }

  PrintMini(x, y, (unsigned char *)text, 0, print_mode);
}

int hal_display_get_width(void) { return 128; /* fx-9860G width */ }

int hal_display_get_height(void) { return 64; /* fx-9860G height */ }

#else

/* Placeholder - will be compiled when USE_CASIO_SDK is defined */
void _casio_display_placeholder(void) {}

#endif /* USE_CASIO_SDK */
