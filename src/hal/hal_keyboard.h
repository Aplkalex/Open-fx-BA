/**
 * Hardware Abstraction Layer - Keyboard Interface
 * Provides unified keyboard API for both fxSDK and Casio SDK
 */

#ifndef HAL_KEYBOARD_H
#define HAL_KEYBOARD_H

/* Normalized key codes (SDK-independent) */
typedef enum {
  HAL_KEY_NONE = 0,

  /* Function keys */
  HAL_KEY_F1,
  HAL_KEY_F2,
  HAL_KEY_F3,
  HAL_KEY_F4,
  HAL_KEY_F5,
  HAL_KEY_F6,

  /* Modifier keys */
  HAL_KEY_SHIFT,
  HAL_KEY_ALPHA,
  HAL_KEY_OPTN,
  HAL_KEY_VARS,

  /* Digits */
  HAL_KEY_0,
  HAL_KEY_1,
  HAL_KEY_2,
  HAL_KEY_3,
  HAL_KEY_4,
  HAL_KEY_5,
  HAL_KEY_6,
  HAL_KEY_7,
  HAL_KEY_8,
  HAL_KEY_9,

  /* Operators */
  HAL_KEY_DOT,
  HAL_KEY_PLUS,
  HAL_KEY_MINUS,
  HAL_KEY_MUL,
  HAL_KEY_DIV,
  HAL_KEY_POW,
  HAL_KEY_NEG,

  /* Navigation */
  HAL_KEY_UP,
  HAL_KEY_DOWN,
  HAL_KEY_LEFT,
  HAL_KEY_RIGHT,

  /* Special */
  HAL_KEY_EXE,
  HAL_KEY_AC,
  HAL_KEY_DEL,
  HAL_KEY_EXIT,
  HAL_KEY_MENU
} HAL_Key;

/* Keyboard functions */

/**
 * Get the currently pressed key (non-blocking)
 * @return Key code or HAL_KEY_NONE
 */
HAL_Key hal_keyboard_get_key(void);

/**
 * Wait for a key press (blocking)
 * @return Key code
 */
HAL_Key hal_keyboard_wait_key(void);

/**
 * Check if a specific key is currently pressed
 * @param key Key to check
 * @return 1 if pressed, 0 otherwise
 */
int hal_keyboard_is_pressed(HAL_Key key);

#endif /* HAL_KEYBOARD_H */
