/**
 * Hardware Abstraction Layer - System Interface
 * Provides unified system API for both fxSDK and Casio SDK
 */

#ifndef HAL_SYSTEM_H
#define HAL_SYSTEM_H

/**
 * Initialize the HAL system
 * Must be called before any other HAL functions
 */
void hal_system_init(void);

/**
 * Shutdown the HAL system
 */
void hal_system_shutdown(void);

/**
 * Sleep for specified milliseconds
 * @param ms Milliseconds to sleep
 */
void hal_system_sleep(int ms);

/**
 * Get current time in milliseconds
 * @return Time in ms (for timing/delays)
 */
unsigned long hal_system_get_time_ms(void);

#endif /* HAL_SYSTEM_H */
