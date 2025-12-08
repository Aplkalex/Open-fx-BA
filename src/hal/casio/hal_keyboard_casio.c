/**
 * Casio SDK Keyboard Adapter
 * Implements HAL keyboard interface using official Casio SDK
 */

#include "../hal_keyboard.h"

#ifdef USE_CASIO_SDK
#include <fxlib.h>

/* Key mapping from Casio SDK to HAL */
static HAL_Key map_casio_key(unsigned int key_code) {
  switch (key_code) {
  case KEY_CTRL_F1:
    return HAL_KEY_F1;
  case KEY_CTRL_F2:
    return HAL_KEY_F2;
  case KEY_CTRL_F3:
    return HAL_KEY_F3;
  case KEY_CTRL_F4:
    return HAL_KEY_F4;
  case KEY_CTRL_F5:
    return HAL_KEY_F5;
  case KEY_CTRL_F6:
    return HAL_KEY_F6;

  case KEY_CTRL_SHIFT:
    return HAL_KEY_SHIFT;
  case KEY_CTRL_ALPHA:
    return HAL_KEY_ALPHA;
  case KEY_CTRL_OPTN:
    return HAL_KEY_OPTN;
  case KEY_CTRL_VARS:
    return HAL_KEY_VARS;

  case KEY_CHAR_0:
    return HAL_KEY_0;
  case KEY_CHAR_1:
    return HAL_KEY_1;
  case KEY_CHAR_2:
    return HAL_KEY_2;
  case KEY_CHAR_3:
    return HAL_KEY_3;
  case KEY_CHAR_4:
    return HAL_KEY_4;
  case KEY_CHAR_5:
    return HAL_KEY_5;
  case KEY_CHAR_6:
    return HAL_KEY_6;
  case KEY_CHAR_7:
    return HAL_KEY_7;
  case KEY_CHAR_8:
    return HAL_KEY_8;
  case KEY_CHAR_9:
    return HAL_KEY_9;

  case KEY_CHAR_DP:
    return HAL_KEY_DOT;
  case KEY_CHAR_PLUS:
    return HAL_KEY_PLUS;
  case KEY_CHAR_MINUS:
    return HAL_KEY_MINUS;
  case KEY_CHAR_MULT:
    return HAL_KEY_MUL;
  case KEY_CHAR_DIV:
    return HAL_KEY_DIV;
  case KEY_CHAR_POW:
    return HAL_KEY_POW;
  case KEY_CTRL_ANS:
    return HAL_KEY_NEG;

  case KEY_CTRL_UP:
    return HAL_KEY_UP;
  case KEY_CTRL_DOWN:
    return HAL_KEY_DOWN;
  case KEY_CTRL_LEFT:
    return HAL_KEY_LEFT;
  case KEY_CTRL_RIGHT:
    return HAL_KEY_RIGHT;

  case KEY_CTRL_EXE:
    return HAL_KEY_EXE;
  case KEY_CTRL_AC:
    return HAL_KEY_AC;
  case KEY_CTRL_DEL:
    return HAL_KEY_DEL;
  case KEY_CTRL_EXIT:
    return HAL_KEY_EXIT;
  case KEY_CTRL_MENU:
    return HAL_KEY_MENU;

  default:
    return HAL_KEY_NONE;
  }
}

HAL_Key hal_keyboard_get_key(void) {
  unsigned int key;
  int result = GetKey(&key);
  if (result == 0) {
    return map_casio_key(key);
  }
  return HAL_KEY_NONE;
}

HAL_Key hal_keyboard_wait_key(void) {
  unsigned int key;
  GetKey(&key);
  return map_casio_key(key);
}

int hal_keyboard_is_pressed(HAL_Key key) {
  /* Casio SDK doesn't have easy key state check */
  /* Would need to poll GetKey non-blocking */
  (void)key;
  return 0;
}

#else

/* Placeholder */
void _casio_keyboard_placeholder(void) {}

#endif /* USE_CASIO_SDK */
