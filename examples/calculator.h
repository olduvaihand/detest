/******************************************************************************
 * calculator.h                                                               *
 ******************************************************************************/
#ifndef __CALCULATOR_H__
#define __CALCULATOR_H__
#include <stdbool.h>

#define MEMORY_CELLS 16

typedef struct calculator {
  double memory[MEMORY_CELLS];
  bool memory_used[MEMORY_CELLS];
  double current;
} calculator;

calculator* calculator_new(void);
calculator* calculator_init(calculator* calc);
calculator* calculator_free(calculator* calc);

bool calculator_add(calculator* calc, double value, double* sum);
bool calculator_subtract(calculator* calc, double value, double* difference);
bool calculator_multiply(calculator* calc, double value, double* product);
bool calculator_divide(calculator* calc, double value, double* quotient);

bool calculator_broken_op(calculator* calc, double value, double* quotient);

bool calculator_addm(calculator* calc, int index, double* sum);
bool calculator_subtractm(calculator* calc, int index, double* difference);
bool calculator_multiplym(calculator* calc, int index, double* product);
bool calculator_dividem(calculator* calc, int index, double* quotient);

double calculator_get_current(calculator* calc);
void calculator_clear(calculator* calc);

bool calculator_store(calculator* calc, int cell, double value);
bool calculator_recall(calculator* calc, int cell, double* value);
bool calculator_clear_memory(calculator* calc, int cell);

void calculator_reset(calculator* calc);

#endif /* __CALCULATOR_H__ */

