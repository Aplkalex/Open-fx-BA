/**
 * Open fx-BA: TI BA II Plus Clone
 * memory.h - 10 User Memory Registers (M0-M9)
 */

#ifndef MEMORY_H
#define MEMORY_H

#define MAX_MEMORIES 10

/* ============================================================
 * Memory Register Operations
 * ============================================================ */
typedef struct {
  double values[MAX_MEMORIES]; /* M0 through M9 */
} MemoryRegisters;

/**
 * Initialize all memory registers to 0.
 */
void memory_init(MemoryRegisters *mem);

/**
 * Store a value in a memory register.
 * @param index Register index (0-9)
 * @param value Value to store
 */
void memory_store(MemoryRegisters *mem, int index, double value);

/**
 * Recall a value from a memory register.
 * @param index Register index (0-9)
 * @return Stored value
 */
double memory_recall(MemoryRegisters *mem, int index);

/**
 * Add to a memory register (M+).
 * @param index Register index (0-9)
 * @param value Value to add
 */
void memory_add(MemoryRegisters *mem, int index, double value);

/**
 * Subtract from a memory register (M-).
 * @param index Register index (0-9)
 * @param value Value to subtract
 */
void memory_subtract(MemoryRegisters *mem, int index, double value);

/**
 * Clear a single memory register.
 */
void memory_clear_one(MemoryRegisters *mem, int index);

/**
 * Clear all memory registers.
 */
void memory_clear_all(MemoryRegisters *mem);

/**
 * Sum all memory registers.
 * @return Sum of M0 through M9
 */
double memory_sum_all(MemoryRegisters *mem);

#endif /* MEMORY_H */
