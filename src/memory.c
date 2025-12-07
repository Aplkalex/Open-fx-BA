/**
 * Open fx-BA: TI BA II Plus Clone
 * memory.c - 10 User Memory Registers implementation
 */

#include "memory.h"
#include <string.h>

void memory_init(MemoryRegisters *mem) {
  memset(mem->values, 0, sizeof(mem->values));
}

void memory_store(MemoryRegisters *mem, int index, double value) {
  if (index >= 0 && index < MAX_MEMORIES) {
    mem->values[index] = value;
  }
}

double memory_recall(MemoryRegisters *mem, int index) {
  if (index >= 0 && index < MAX_MEMORIES) {
    return mem->values[index];
  }
  return 0.0;
}

void memory_add(MemoryRegisters *mem, int index, double value) {
  if (index >= 0 && index < MAX_MEMORIES) {
    mem->values[index] += value;
  }
}

void memory_subtract(MemoryRegisters *mem, int index, double value) {
  if (index >= 0 && index < MAX_MEMORIES) {
    mem->values[index] -= value;
  }
}

void memory_clear_one(MemoryRegisters *mem, int index) {
  if (index >= 0 && index < MAX_MEMORIES) {
    mem->values[index] = 0.0;
  }
}

void memory_clear_all(MemoryRegisters *mem) { memory_init(mem); }

double memory_sum_all(MemoryRegisters *mem) {
  double sum = 0.0;
  for (int i = 0; i < MAX_MEMORIES; i++) {
    sum += mem->values[i];
  }
  return sum;
}
