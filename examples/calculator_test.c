/******************************************************************************
 * calculator_test.c                                                          *
 ******************************************************************************/
#include "detest.h"
#include "calculator.h"

Detest_TestSuiteSetupFn() {
  env->user_data = (void*) calculator_init(calculator_new(), "Casio");
}

Detest_TestSuiteTeardownFn() {
  env->user_data = calculator_free((calculator*) env->user_data);
}

Detest_TestSetupFn() {
  calculator_reset((calculator*) env->user_data);
}

Detest_TestFn(get_brand) {
  calculator* calc = (calculator*) env->user_data;
  Detest_AssertNotNull(calc);

  char* brand;
  Detest_AssertTrue(calculator_get_brand(calc, &brand));
  Detest_AssertStringsEqual(brand, "Casio");
  Detest_AssertStringsNotEqual(brand, "TI");
  Detest_AssertStringsEqualN(brand, "Cas", 3);
  Detest_AssertStringsNotEqualN(brand, "TI", 2);
}

Detest_TestFn(operators) {
  calculator* calc = (calculator*) env->user_data;
  double result;
  Detest_AssertNotNull(calc);

  Detest_AssertTrue(calculator_add(calc, 10., &result));
  Detest_AssertEqual(10., result);
  Detest_AssertEqual(calculator_get_current(calc), result);
  Detest_AssertTrue(calculator_subtract(calc, 5., &result));
  Detest_AssertEqual(5., result);
  Detest_AssertEqual(calculator_get_current(calc), result);
  Detest_AssertTrue(calculator_multiply(calc, 2., &result));
  Detest_AssertEqual(10., result);
  Detest_AssertEqual(calculator_get_current(calc), result);
  Detest_AssertTrue(calculator_divide(calc, 5., &result));
  Detest_AssertEqual(2., result);
  Detest_AssertEqual(calculator_get_current(calc), result);
}

Detest_TestFn(broken_op) {
  calculator* calc = (calculator*) env->user_data;
  double result;
  Detest_AssertNotNull(calc);

  Detest_AssertTrue(calculator_broken_op(calc, 42., &result));
}

Detest_TestFn(memory_operators) {
  calculator* calc = (calculator*) env->user_data;
  double result;
  Detest_AssertNotNull(calc);

  Detest_AssertTrue(calculator_store(calc, 0, 10.));
  Detest_AssertTrue(calculator_store(calc, 1, 5.));
  Detest_AssertTrue(calculator_store(calc, 2, 2.));
  Detest_AssertTrue(calculator_store(calc, 3, 5.));

  Detest_AssertTrue(calculator_addm(calc, 0, &result));
  Detest_AssertEqual(10., result);
  Detest_AssertEqual(calculator_get_current(calc), result);
  Detest_AssertTrue(calculator_subtractm(calc, 1, &result));
  Detest_AssertEqual(5., result);
  Detest_AssertEqual(calculator_get_current(calc), result);
  Detest_AssertTrue(calculator_multiplym(calc, 2, &result));
  Detest_AssertEqual(10., result);
  Detest_AssertEqual(calculator_get_current(calc), result);
  Detest_AssertTrue(calculator_dividem(calc, 3, &result));
  Detest_AssertEqual(2., result);
  Detest_AssertEqual(calculator_get_current(calc), result);
}

Detest_TestFn(memory) {
  calculator* calc = (calculator*) env->user_data;
  double result;
  Detest_AssertNotNull(calc);

  Detest_AssertFalse(calculator_recall(calc, 0, &result));
  Detest_AssertFalse(calculator_recall(calc, 3, &result));
  Detest_AssertFalse(calculator_recall(calc, 8, &result));
  Detest_AssertFalse(calculator_recall(calc, MEMORY_CELLS-1, &result));

  Detest_AssertTrue(calculator_store(calc, 0, 10.));
  Detest_AssertTrue(calculator_store(calc, 3, 3.14));
  Detest_AssertTrue(calculator_store(calc, 8, 42));
  Detest_AssertTrue(calculator_store(calc, MEMORY_CELLS-1, 2.71828));

  Detest_AssertTrue(calculator_recall(calc, 0, &result));
  Detest_AssertEqual(10., result);
  Detest_AssertTrue(calculator_recall(calc, 3, &result));
  Detest_AssertEqual(3.14, result);
  Detest_AssertTrue(calculator_recall(calc, 8, &result));
  Detest_AssertEqual(42, result);
  Detest_AssertTrue(calculator_recall(calc, MEMORY_CELLS-1, &result));
  Detest_AssertEqual(2.71828, result);

  Detest_AssertTrue(calculator_clear_memory(calc, 0));
  Detest_AssertTrue(calculator_clear_memory(calc, 3));
  Detest_AssertTrue(calculator_clear_memory(calc, 8));
  Detest_AssertTrue(calculator_clear_memory(calc, MEMORY_CELLS-1));

  Detest_AssertFalse(calculator_recall(calc, 0, &result));
  Detest_AssertFalse(calculator_recall(calc, 3, &result));
  Detest_AssertFalse(calculator_recall(calc, 8, &result));
  Detest_AssertFalse(calculator_recall(calc, MEMORY_CELLS-1, &result));
}

Detest_TestSuite_WithSuiteAndTestSetupAndTeardownCustom(
    Detest_TestSuiteSetup,
    Detest_TestSuiteTeardown,
    Detest_TestSetup,
    Detest_Noop,
    Detest_Test(get_brand),
    Detest_Test(operators),
    Detest_Test(broken_op),
    Detest_Test(memory_operators),
    Detest_Test(memory))
