/**
 * fxSDK Keyboard Adapter
 * Implements HAL keyboard interface using fxSDK/gint
 */

#include "../hal_keyboard.h"

#ifdef USE_FXSDK
#include <gint/keyboard.h>

/* Key mapping from fxSDK to HAL */
static HAL_Key map_key(key_event_t event) {
  if (event.type != KEYEV_DOWN)
    return HAL_KEY_NONE;

  switch (event.key) {
  case KEY_F1:
    return HAL_KEY_F1;
  case KEY_F2:
    return HAL_KEY_F2;
  case KEY_F3:
    return HAL_KEY_F3;
  case KEY_F4:
    return HAL_KEY_F4;
  case KEY_F5:
    return HAL_KEY_F5;
  case KEY_F6:
    return HAL_KEY_F6;

  case KEY_SHIFT:
    return HAL_KEY_SHIFT;
  case KEY_ALPHA:
    return HAL_KEY_ALPHA;
  case KEY_OPTN:
    return HAL_KEY_OPTN;
  case KEY_VARS:
    return HAL_KEY_VARS;

  case KEY_0:
    return HAL_KEY_0;
  case KEY_1:
    return HAL_KEY_1;
  case KEY_2:
    return HAL_KEY_2;
  case KEY_3:
    return HAL_KEY_3;
  case KEY_4:
    return HAL_KEY_4;
  case KEY_5:
    return HAL_KEY_5;
  case KEY_6:
    return HAL_KEY_6;
  case KEY_7:
    return HAL_KEY_7;
  case KEY_8:
    return HAL_KEY_8;
  case KEY_9:
    return HAL_KEY_9;

  case KEY_DOT:
    return HAL_KEY_DOT;
  case KEY_PLUS:
    return HAL_KEY_PLUS;
  case KEY_MINUS:
    return HAL_KEY_MINUS;
  case KEY_MUL:
    return HAL_KEY_MUL;
  case KEY_DIV:
    return HAL_KEY_DIV;
  case KEY_POW:
    return HAL_KEY_POW;
  case KEY_NEG:
    return HAL_KEY_NEG;

  case KEY_UP:
    return HAL_KEY_UP;
  case KEY_DOWN:
    return HAL_KEY_DOWN;
  case KEY_LEFT:
    return HAL_KEY_LEFT;
  case KEY_RIGHT:
    return HAL_KEY_RIGHT;

  case KEY_EXE:
    return HAL_KEY_EXE;
  case KEY_AC:
    return HAL_KEY_AC;
  case KEY_DEL:
    return HAL_KEY_DEL;
  case KEY_EXIT:
    return HAL_KEY_EXIT;
  case KEY_MENU:
    return HAL_KEY_MENU;

  default:
    return HAL_KEY_NONE;
  }
}

HAL_Key hal_keyboard_get_key(void) {
  key_event_t event = pollevent();
  return map_key(event);
}

HAL_Key hal_keyboard_wait_key(void) {
  key_event_t event = getkey();
  return map_key(event);
}

int hal_keyboard_is_pressed(HAL_Key key) {
  /* Simple poll for key state */
  key_event_t event = pollevent();
  return (map_key(event) == key) ? 1 : 0;
}

#else

/* Stub implementations */
HAL_Key hal_keyboard_get_key(void) { return HAL_KEY_NONE; }

HAL_Key hal_keyboard_wait_key(void) { return HAL_KEY_EXE; }

int hal_keyboard_is_pressed(HAL_Key key) {
  (void)key;
  return 0;
}

#endif /* USE_FXSDK */
