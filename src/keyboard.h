/**
 * Open fx-BA: TI BA II Plus Clone
 * keyboard.h - TI-style keyboard handling
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "screens.h"
#include "types.h"

/* ============================================================
 * Key Actions
 * ============================================================ */
typedef enum {
  ACTION_NONE,
  ACTION_DIGIT,
  ACTION_DECIMAL,
  ACTION_NEGATIVE,
  ACTION_BACKSPACE,
  ACTION_CLEAR,
  ACTION_CLEAR_ALL,
  ACTION_STORE_VAR,
  ACTION_COMPUTE_VAR,
  ACTION_CPT,
  ACTION_2ND,
  ACTION_STO,          /* STO key - store to memory M0-M9 */
  ACTION_RCL,          /* RCL key - recall from memory M0-M9 */
  ACTION_ENTER,
  ACTION_UP,
  ACTION_DOWN,
  ACTION_LEFT,
  ACTION_RIGHT,
  ACTION_MENU_F1,
  ACTION_MENU_F2,
  ACTION_MENU_F3,
  ACTION_MENU_F4,
  ACTION_MENU_F5,
  ACTION_MENU_F6,
  ACTION_SWITCH_SCREEN,
  ACTION_EXIT
} KeyAction;

/* ============================================================
 * Key Event Structure
 * ============================================================ */
typedef struct {
  KeyAction action;
  char digit;        /* For ACTION_DIGIT */
  int fkeyIndex;     /* For F-key actions (0-5) */
  ScreenType screen; /* For ACTION_SWITCH_SCREEN */
} KeyEvent;

/* ============================================================
 * Keyboard Functions
 * ============================================================ */

/**
 * Process a raw key code and return structured event.
 * Handles 2nd function state automatically.
 */
KeyEvent keyboard_process(int keyCode, ScreenState *state);

/**
 * Handle the key event and update calculator state.
 * Returns 1 if app should exit.
 */
int keyboard_handle_event(KeyEvent *event, Calculator *calc,
                          ScreenState *state);

/**
 * Check if SHIFT/2nd key is pressed
 */
int keyboard_is_shift_pressed(void);

#endif /* KEYBOARD_H */
