/**
 * Hardware Abstraction Layer - Display Interface
 * Provides unified display API for both fxSDK and Casio SDK
 */

#ifndef HAL_DISPLAY_H
#define HAL_DISPLAY_H

/* Display modes for text rendering */
typedef enum {
  HAL_TEXT_NORMAL = 0,     /* Normal text */
  HAL_TEXT_REVERSE = 1,    /* Inverted (white on black) */
  HAL_TEXT_TRANSPARENT = 2 /* Transparent background */
} HAL_TextMode;

/* Display functions */

/**
 * Clear the entire screen
 */
void hal_display_clear(void);

/**
 * Refresh/update the display (push buffer to screen)
 */
void hal_display_refresh(void);

/**
 * Print text at specified position
 * @param x X coordinate (pixels)
 * @param y Y coordinate (pixels)
 * @param text Text to print (null-terminated)
 * @param mode Text rendering mode
 */
void hal_display_print_text(int x, int y, const char *text, HAL_TextMode mode);

/**
 * Get display width in pixels
 */
int hal_display_get_width(void);

/**
 * Get display height in pixels
 */
int hal_display_get_height(void);

/**
 * Set a single pixel
 * @param x X coordinate
 * @param y Y coordinate
 * @param color 1 = black/on, 0 = white/off
 */
void hal_display_set_pixel(int x, int y, int color);

/**
 * Reverse (invert) an area of the display
 * @param x1, y1 Top-left corner
 * @param x2, y2 Bottom-right corner
 */
void hal_display_area_reverse(int x1, int y1, int x2, int y2);

#endif /* HAL_DISPLAY_H */
