/**
 * fxSDK System Adapter
 * Implements HAL system interface using fxSDK/gint
 */

#include "../hal_system.h"

#ifdef USE_FXSDK
#include <gint/clock.h>
#include <gint/gint.h>

void hal_system_init(void) { /* gint initializes automatically in main */ }

void hal_system_shutdown(void) { /* gint handles shutdown */ }

void hal_system_sleep(int ms) {
  /* Use a simple busy wait or timer */
  (void)ms;
  /* sleep() is deprecated, use timer or just skip */
}

unsigned long hal_system_get_time_ms(void) {
  /* Return 0 for now - timing not critical for calculator */
  return 0;
}

#else

/* Stub implementations */
#include <stdio.h>

void hal_system_init(void) { printf("[HAL] System initialized\n"); }

void hal_system_shutdown(void) { printf("[HAL] System shutdown\n"); }

void hal_system_sleep(int ms) {
  /* No-op in stub */
  (void)ms;
}

unsigned long hal_system_get_time_ms(void) { return 0; }

#endif /* USE_FXSDK */
