/**
 * fxSDK System Adapter
 * Implements HAL system interface using fxSDK/gint
 */

#include "../hal_system.h"

#ifdef USE_FXSDK
#include <gint/gint.h>
#include <gint/rtc.h>
#include <gint/timer.h>

void hal_system_init(void) { /* gint initializes automatically in main */ }

void hal_system_shutdown(void) { /* gint handles shutdown */ }

void hal_system_sleep(int ms) { sleep_ms(ms); }

unsigned long hal_system_get_time_ms(void) {
  rtc_time_t time = rtc_time();
  return (unsigned long)(time.ticks * 4); /* Approximate ms */
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
