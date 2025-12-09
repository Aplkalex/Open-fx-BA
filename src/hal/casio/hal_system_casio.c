/**
 * Casio SDK System Adapter
 * Implements HAL system interface using official Casio SDK
 */

#include "../hal_system.h"

#ifdef USE_CASIO_SDK
#include <fxlib.h>

void hal_system_init(void) { /* Casio SDK initializes in AddIn_main */ }

void hal_system_shutdown(void) { /* Return from AddIn_main */ }

void hal_system_sleep(int ms) {
  /* Casio SDK sleep function */
  Sleep(ms);
}

unsigned long hal_system_get_time_ms(void) {
  /* Casio SDK RTC access */
  return RTC_GetTicks() / 128; /* Approximate ms from ticks */
}

#else

/* Stub implementations for non-Casio builds */
void hal_system_init(void) {}

void hal_system_shutdown(void) {}

void hal_system_sleep(int ms) { (void)ms; }

unsigned long hal_system_get_time_ms(void) { return 0; }

#endif /* USE_CASIO_SDK */
