/**
 * Open fx-BA: TI BA II Plus Clone
 * fonts.h - Large 7-segment style digits for authentic calculator look
 */

#ifndef FONTS_H
#define FONTS_H

/* ============================================================
 * 7-Segment Large Font (8x12 pixels per digit)
 * ============================================================ */

#define LARGE_CHAR_WIDTH 8
#define LARGE_CHAR_HEIGHT 12

/**
 * Draw a single large digit (0-9) at position.
 */
void font_draw_digit(int x, int y, char digit);

/**
 * Draw a large decimal point.
 */
void font_draw_decimal(int x, int y);

/**
 * Draw a large minus sign.
 */
void font_draw_minus(int x, int y);

/**
 * Draw a complete number string with large font.
 * Right-aligned from x position.
 * @param rightX Right edge X position
 * @param y Y position
 * @param str Number string (e.g., "-1,234.56")
 */
void font_draw_number(int rightX, int y, const char *str);

/**
 * Calculate pixel width of a number string in large font.
 */
int font_get_width(const char *str);

/* ============================================================
 * Medium Font (for F-key labels with better spacing)
 * ============================================================ */

#define MEDIUM_CHAR_WIDTH 6
#define MEDIUM_CHAR_HEIGHT 8

/**
 * Draw text with medium font (better than PrintMini for menus).
 */
void font_draw_medium(int x, int y, const char *str);

/**
 * Draw F-key label with box background.
 * @param index F-key index (0-5)
 * @param label Label text
 * @param highlighted 1 for reverse video (2nd function active)
 */
void font_draw_fkey_label(int index, const char *label, int highlighted);

/* ============================================================
 * Status Bar Icons (tiny indicators)
 * ============================================================ */

/**
 * Draw 2ND indicator (small box with "2").
 */
void icon_draw_2nd(int x, int y, int active);

/**
 * Draw CPT indicator.
 */
void icon_draw_cpt(int x, int y, int blinking);

/**
 * Draw BGN indicator (Begin mode).
 */
void icon_draw_bgn(int x, int y, int active);

/**
 * Draw battery/memory indicator.
 */
void icon_draw_memory(int x, int y, int memIndex);

#endif /* FONTS_H */
