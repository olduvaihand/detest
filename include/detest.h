/******************************************************************************
 * detest.h                                                                   *
 ******************************************************************************/
#ifndef __DETEST_H__
#define __DETEST_H__
#include <setjmp.h>
#include <stdio.h>
#include <time.h>

#define DETEST_MAX_TEST_SUITE_SIZE 1024

typedef enum detest_status {
  DETEST_STATUS_PASS,
  DETEST_STATUS_FAIL
} detest_status;

typedef struct detest_environment detest_environment;
typedef struct detest_test_suite detest_test_suite;
typedef struct detest_test detest_test;

typedef void (*detest_test_fn)(detest_test* test, detest_environment* env);

struct detest_test {
  char* name;
  detest_test_fn test_fn;
  char* message;
  detest_status status;
};

struct detest_test_suite {
  detest_test tests[DETEST_MAX_TEST_SUITE_SIZE];
  detest_test passing[DETEST_MAX_TEST_SUITE_SIZE];
  detest_test failing[DETEST_MAX_TEST_SUITE_SIZE];
};

struct detest_environment {
  jmp_buf jump_buffer;
  time_t start_time;
};

#define Detest_Assert(expr, message_) \
  { if (!(expr)) { \
    test->message = message_; \
    test->status = DETEST_STATUS_FAIL; \
    longjmp(env->jump_buffer, DETEST_STATUS_FAIL); \
  } }
#define Detest_AssertTrue(expr) Detest_Assert(expr, #expr" is false.")
#define Detest_AssertFalse(expr) Detest_Assert(!(expr), #expr " is true.")
#define Detest_AssertEqual(a, b) Detest_Assert(a == b, #a " != " #b)
#define Detest_AssertNotEqual(a, b) Detest_Assert(a != b, #a " == " #b)
#define Detest_AssertNull(ptr) Detest_Assert(ptr == NULL, #ptr " == NULL")
#define Detest_AssertNotNull(ptr) Detest_Assert(ptr != NULL, #ptr " != NULL")
#define Detest_AssertInRange(value, low, high) \
  Detest_Assert(low <= value && value <= high, #value " not in range [" #low ", " #high)

#define Detest_Test(test_name) { .name = #test_name, .test_fn = &test_name##_fn }

#define Detest_TestSuite(...) \
static detest_test_suite test_suite = { \
  .tests = { __VA_ARGS__ } \
}; \
\
int main(int argc, char* argv[]) { \
  detest_environment env; \
  detest_status status; \
  detest_test* passing = test_suite.passing; \
  detest_test* failing = test_suite.failing; \
  for (detest_test* test = test_suite.tests; test->name != NULL; test++) { \
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
  } \
  printf("\n\n"); \
  printf("Passing:\n"); \
  printf("---------------------------------------------------------------\n"); \
  for (detest_test* test = test_suite.passing; test->name != NULL; test++) { \
    printf("%s\n", test->name); \
  } \
  printf("===============================================================\n\n"); \
  printf("Failures:\n"); \
  printf("---------------------------------------------------------------\n"); \
  for (detest_test* test = test_suite.failing; test->name != NULL; test++) { \
    printf("%s: %s\n", test->name, test->message); \
  } \
  printf("===============================================================\n\n"); \
  return 0; \
}

#endif /* __DETEST_H__ */
