/******************************************************************************
 * detest.h                                                                   *
 *                                                                            *
 * Copyright (c) 2015 Ben Trofatter <trofatter@google.com>                    *
 * This header is part of Detest and is released under the MIT License:       *
 * http://www.opensource.org/licenses/mit-license.php                         *
 ******************************************************************************/
#ifndef __DETEST_H__
#define __DETEST_H__
#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>

#define DETEST_MAX_TEST_SUITE_SIZE 1024

typedef enum detest_status {
  DETEST_STATUS_PASS,
  DETEST_STATUS_FAIL
} detest_status;

typedef struct detest_environment detest_environment;
typedef struct detest_test_suite detest_test_suite;
typedef struct detest_test detest_test;

typedef void (*detest_test_suite_setup_fn)(detest_environment* env);
typedef void (*detest_test_suite_teardown_fn)(detest_environment* env);
typedef void (*detest_test_setup_fn)(detest_environment* env);
typedef void (*detest_test_teardown_fn)(detest_environment* env);
typedef void (*detest_test_fn)(detest_test* test, detest_environment* env);

void detest_initialize(detest_environment* env);
int detest_finalize(detest_environment* env);
double detest_elapsed_time(detest_environment* env);
void detest_noop(detest_environment* env);

// Implementations

struct detest_test {
  char* name;
  detest_test_fn test_fn;
  char* message;
};

struct detest_test_suite {
  detest_test_suite_setup_fn suite_setup;
  detest_test_suite_teardown_fn suite_teardown;
  detest_test_setup_fn test_setup;
  detest_test_teardown_fn test_teardown;
  detest_test tests[DETEST_MAX_TEST_SUITE_SIZE];
};

struct detest_environment {
  jmp_buf jump_buffer;
  struct timeval start_time;
  struct timeval end_time;
  detest_test passing[DETEST_MAX_TEST_SUITE_SIZE];
  detest_test failing[DETEST_MAX_TEST_SUITE_SIZE];
  void* user_data;
};

void detest_initialize(detest_environment* env) {
  gettimeofday(&env->start_time, NULL);
}

int detest_finalize(detest_environment* env) {
  gettimeofday(&env->end_time, NULL);
  bool failed = false;
  printf("\n\n");
  if (env->passing->name != NULL) {
    printf("Passing:\n");
    printf("---------------------------------------------------------------\n");
    for (detest_test* test = env->passing; test->name != NULL; test++) {
      printf("%s\n", test->name);
    }
    printf("===============================================================\n\n");
  }
  if (env->failing->name != NULL) {
    failed = true;
    printf("Failures:\n");
    printf("---------------------------------------------------------------\n");
    for (detest_test* test = env->failing; test->name != NULL; test++) {
      printf("%s: %s\n", test->name, test->message);
    }
    printf("===============================================================\n\n");
  }
  double elapsed_time = detest_elapsed_time(env);
  printf("Detest completed in %f seconds.\n", elapsed_time);
  return (failed ? DETEST_STATUS_FAIL : DETEST_STATUS_PASS);
}

double detest_elapsed_time(detest_environment* env) {
  int start_time = 100000 * env->start_time.tv_sec + env->start_time.tv_usec;
  int end_time = 100000 * env->end_time.tv_sec + env->end_time.tv_usec;
  return (end_time - start_time) / 100000.;
}

void detest_noop(detest_environment* env) {}

// Macros

// Assertions

#define Detest_Assert(expr, message_) \
  { if (!(expr)) { \
    test->message = message_; \
    longjmp(env->jump_buffer, DETEST_STATUS_FAIL); \
  } }
#define Detest_AssertTrue(expr) Detest_Assert(expr, #expr" is false.")
#define Detest_AssertFalse(expr) Detest_Assert(!(expr), #expr" is true.")
#define Detest_AssertEqual(a, b) Detest_Assert(a == b, #a" != "#b)
#define Detest_AssertNotEqual(a, b) Detest_Assert(a != b, #a" == "#b)
#define Detest_AssertNull(ptr) Detest_Assert(ptr == NULL, #ptr" == NULL")
#define Detest_AssertNotNull(ptr) Detest_Assert(ptr != NULL, #ptr" != NULL")
#define Detest_AssertInRange(value, low, high) \
  Detest_Assert(low <= value && value <= high, #value " not in range ["#low", "#high"]")

// Detest hooks and functions

#define Detest_TestSuiteSetupFn() void test_suite_setup(detest_environment* env)
#define Detest_TestSuiteTeardownFn() void test_suite_teardown(detest_environment* env)
#define Detest_TestSetupFn() void test_setup(detest_environment* env)
#define Detest_TestTeardownFn() void test_teardown(detest_environment* env)
#define Detest_TestFn(name) void name##_fn(detest_test* test, detest_environment* env)

#define Detest_Main() \
int main(int argc, char* argv[]) { \
  detest_environment env; \
  detest_status status; \
  detest_initialize(&env); \
  detest_test* passing = env.passing; \
  detest_test* failing = env.failing; \
  test_suite.suite_setup(&env); \
  for (detest_test* test = test_suite.tests; test->name != NULL; test++) { \
    test_suite.test_setup(&env); \
    switch ((status = setjmp(env.jump_buffer))) { \
      case DETEST_STATUS_FAIL: \
        *failing++ = *test; \
        printf("F"); \
        break; \
      default: \
        test->test_fn(test, &env); \
        *passing++ = *test; \
        printf("."); \
        break; \
    } \
    test_suite.test_teardown(&env); \
  } \
  test_suite.suite_teardown(&env); \
  return detest_finalize(&env); \
}

// Test suites

#define Detest_TestSuiteSetup test_suite_setup
#define Detest_TestSuiteTeardown test_suite_teardown
#define Detest_TestSetup test_setup
#define Detest_TestTeardown test_teardown
#define Detest_Noop detest_noop
#define Detest_Test(test_name) { .name = #test_name, .test_fn = &test_name##_fn }

#define Detest_TestSuite(...) \
static detest_test_suite test_suite = { \
  .suite_setup = &Detest_Noop, \
  .suite_teardown = &Detest_Noop, \
  .test_setup = &Detest_Noop, \
  .test_teardown = &Detest_Noop, \
  .tests = { __VA_ARGS__ } \
}; \
Detest_Main()

#define Detest_TestSuite_WithSuiteSetupAndTeardown(...) \
static detest_test_suite test_suite = { \
  .suite_setup = &Detest_TestSuiteSetup, \
  .suite_teardown = &Detest_TestSuiteTeardown, \
  .test_setup = &Detest_Noop, \
  .test_teardown = &Detest_Noop, \
  .tests = { __VA_ARGS__ } \
}; \
Detest_Main()

#define Detest_TestSuite_WithTestSetupAndTeardown(...) \
static detest_test_suite test_suite = { \
  .suite_setup = &Detest_Noop, \
  .suite_teardown = &Detest_Noop, \
  .test_setup = &Detest_TestSetup, \
  .test_teardown = &Detest_TestTeardown, \
  .tests = { __VA_ARGS__ } \
}; \
Detest_Main()

#define Detest_TestSuite_WithSuiteAndTestSetupAndTeardown(...) \
static detest_test_suite test_suite = { \
  .suite_setup = &Detest_TestSuiteSetup, \
  .suite_teardown = &Detest_TestSuiteTeardown, \
  .test_setup = &Detest_TestSetup, \
  .test_teardown = &Detest_TestTeardown, \
  .tests = { __VA_ARGS__ } \
}; \
Detest_Main()

#define Detest_TestSuite_WithSuiteAndTestSetupAndTeardownCustom( \
    suite_setup_fn, suite_teardown_fn, test_setup_fn, test_teardown_fn, ...) \
static detest_test_suite test_suite = { \
  .suite_setup = &suite_setup_fn, \
  .suite_teardown = &suite_teardown_fn, \
  .test_setup = &test_setup_fn, \
  .test_teardown = &test_teardown_fn, \
  .tests = { __VA_ARGS__ } \
}; \
Detest_Main()

#endif /* __DETEST_H__ */
