/******************************************************************************
 * calculator.c                                                               *
 ******************************************************************************/
#include <stdlib.h>
#include "calculator.h"

calculator* calculator_new() {
  return (calculator*) malloc(sizeof(calculator));
}

calculator* calculator_init(calculator* calc) {
  calculator_reset(calc);
  return calc;
}

calculator* calculator_free(calculator* calc) {
  free(calc);
  return NULL;
}

bool calculator_add(calculator* calc, double value, double* sum) {
  *sum = calc->current += value;
  return true;
}

bool calculator_subtract(calculator* calc, double value, double* difference) {
  *difference = calc->current -= value;
  return true;
}

bool calculator_multiply(calculator* calc, double value, double* product) {
  *product = calc->current *= value;
  return true;
}

bool calculator_divide(calculator* calc, double value, double* quotient) {
  if (value == 0.0) {
    return false;
  }
  *quotient = calc->current /= value;
  return true;
}

bool calculator_broken_op(calculator* calc, double value, double* result) {
  return false;
}

bool calculator_addm(calculator* calc, int index, double* sum) {
  double stored;
  if (!calculator_recall(calc, index, &stored)) {
    return false;
  }
  return calculator_add(calc, stored, sum);
}

bool calculator_subtractm(calculator* calc, int index, double* difference) {
  double stored;
  if (!calculator_recall(calc, index, &stored)) {
    return false;
  }
  return calculator_subtract(calc, stored, difference);
}

bool calculator_multiplym(calculator* calc, int index, double* product) {
  double stored;
  if (!calculator_recall(calc, index, &stored)) {
    return false;
  }
  return calculator_multiply(calc, stored, product);
}

bool calculator_dividem(calculator* calc, int index, double* quotient) {
  double stored;
  if (!calculator_recall(calc, index, &stored)) {
    return false;
  }
  return calculator_divide(calc, stored, quotient);
}

double calculator_get_current(calculator* calc) {
  return calc->current;
}

void calculator_clear(calculator* calc) {
  calc->current = 0.;
}

bool calculator_store(calculator* calc, int cell, double value) {
  if (0 <= cell && cell < MEMORY_CELLS) {
    calc->memory[cell] = value;
    calc->memory_used[cell] = true;
    return true;
  }
  return false;
}

bool calculator_recall(calculator* calc, int cell, double* value) {
  if (0 <= cell && cell < MEMORY_CELLS && calc->memory_used[cell]) {
    *value = calc->memory[cell];
    return true;
  }
  return false;
}

bool calculator_clear_memory(calculator* calc, int cell) {
  if (0 <= cell && cell < MEMORY_CELLS) {
    calc->memory_used[cell] = false;
    return true;
  }
  return false;
}

void calculator_reset(calculator* calc) {
  calculator_clear(calc);
  for (int i=0; i < MEMORY_CELLS; i++) {
    calculator_clear_memory(calc, i);
  }
}
