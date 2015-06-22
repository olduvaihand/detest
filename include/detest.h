/******************************************************************************
 * detest.h                                                                   *
 ******************************************************************************/
#ifndef __DETEST_H__
#define __DETEST_H__
#include <setjmp.h>
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

typedef void (*detest_test_fn)(detest_test* test, detest_environment* env);

struct detest_test {
  char* name;
  detest_test_fn test_fn;
  char* message;
  detest_status status;
};

struct detest_test_suite {
  detest_test tests[DETEST_MAX_TEST_SUITE_SIZE];
};

struct detest_environment {
  jmp_buf jump_buffer;
  struct timeval start_time;
  struct timeval end_time;
  detest_test passing[DETEST_MAX_TEST_SUITE_SIZE];
  detest_test failing[DETEST_MAX_TEST_SUITE_SIZE];
};

void detest_initialize(detest_environment* env) {
  gettimeofday(&env->start_time, NULL);
}

double detest_elapsed_time(detest_environment* env) {
  int start_time = 100000 * env->start_time.tv_sec + env->start_time.tv_usec;
  int end_time = 100000 * env->end_time.tv_sec + env->end_time.tv_usec;
  return (end_time - start_time) / 100000.;
}

int detest_finalize(detest_environment* env) {
  gettimeofday(&env->end_time, NULL);
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
    printf("Failures:\n");
    printf("---------------------------------------------------------------\n");
    for (detest_test* test = env->failing; test->name != NULL; test++) {
      printf("%s: %s\n", test->name, test->message);
    }
    printf("===============================================================\n\n");
  }
  double elapsed_time = detest_elapsed_time(env);
  printf("Detest completed in %f seconds.\n", elapsed_time);
  return 0;
}

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
  detest_initialize(&env); \
  detest_test* passing = env.passing; \
  detest_test* failing = env.failing; \
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
  return detest_finalize(&env); \
}

#endif /* __DETEST_H__ */
