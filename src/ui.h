/**
 * Open fx-BA: TI BA II Plus Clone
 * ui.h - User interface rendering
 */

#ifndef UI_H
#define UI_H

#include "types.h"

/* ============================================================
 * Screen Initialization
 * ============================================================ */
void ui_init(void);
void ui_clear(void);
void ui_refresh(void);

/* ============================================================
 * Status Bar (top line)
 * ============================================================ */
void ui_draw_status_bar(Calculator *calc);
void ui_draw_sto_indicator(int active);
void ui_draw_rcl_indicator(int active);
void ui_draw_2nd_indicator(int active);

/* ============================================================
 * Main Display (center - big number display)
 * ============================================================ */
void ui_draw_main_display(const char *text);
void ui_draw_right_aligned(int y, const char *text);
void ui_draw_display_with_label(const char *label, const char *value);

/* ============================================================
 * F-Key Menu (bottom of screen)
 * ============================================================ */
void ui_draw_fkey_menu(const char *labels[], int count);
void ui_draw_tvm_menu(void);
void ui_draw_cpt_indicator(int active);
void ui_draw_worksheet_hints(int showUp, int showDown);

/* ============================================================
 * Error Display (TI BA II Plus style - in-place)
 * ============================================================ */
void ui_show_error(int errorCode);
void ui_show_error_inline(const char *message);

/* ============================================================
 * Memory Feedback Display
 * ============================================================ */
void ui_show_memory_stored(int index, double value);
void ui_show_memory_recalled(int index, double value);

/* ============================================================
 * Popup Messages
 * ============================================================ */
void ui_show_message(const char *title, const char *message);

/* ============================================================
 * Formatting Helpers
 * ============================================================ */
void format_number(double value, char *buffer, int maxLen);
void format_with_commas(double value, char *buffer, int maxLen);

#endif /* UI_H */
