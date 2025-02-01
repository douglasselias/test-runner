#include "os/dse_windows.c"

dse_s64* dse_total_tests;
dse_s64* dse_total_tests_skipped;
dse_s64* dse_total_tests_failed;

void dse_init_results() {
  dse_total_tests         = calloc(sizeof(dse_u64), 1);
  dse_total_tests_skipped = calloc(sizeof(dse_u64), 1);
  dse_total_tests_failed  = calloc(sizeof(dse_u64), 1);

  *dse_total_tests         = 0;
  *dse_total_tests_skipped = 0;
  *dse_total_tests_failed  = 0;
}

void dse_print_results() {
  dse_u64 total = *dse_total_tests - *dse_total_tests_skipped - *dse_total_tests_failed;
  printf(
    "\n"
    "Total tests:\t%lld\n"
    "Total skipped:\t%lld\n"
    "Total failed:\t%lld\n"
    "Total passed:\t%lld\n"
    ,
    *dse_total_tests,
    *dse_total_tests_skipped,
    *dse_total_tests_failed,
    total
    );
}

/// @todo: Can I expand the macros before compiling? The assert is not returning a useful line number.
#define DSE_ASSERT(expression, ...) \
  dse_atomic_increment(dse_total_tests); \
  if(expression) { \
    printf("\033[32mPASSED\033[0m\t"); \
    printf("Line: %s:%d\n", __FILE__, __LINE__); \
  } else { \
    dse_atomic_increment(dse_total_tests_failed); \
    printf("\033[31mFAILED\033[0m\t"); \
    printf("Line: %s:%d  ", __FILE__, __LINE__); \
    printf("" __VA_ARGS__); \
    puts(""); \
  } \

#define DSE_SKIP(expression_not_used) \
  dse_atomic_increment(dse_total_tests_skipped); \
  dse_atomic_increment(dse_total_tests); \
  printf("\033[93mSKIPPED\033[0m\tLine: %s:%d\n", __FILE__, __LINE__); \

/// @todo: Think a better way to filter the tests and suites. Specially the tests inside the suites.
// char dse_query[50] = {0};
char* dse_query = "";

/// @todo: I could filter the tests when generating the file, instead of having the if statement on the macro.
#define DSE_SUITE(name, code) \
  void dse_##name() { \
    if(dse_has_substring(#name, dse_query)) { \
      printf("\033[95mSUITE %s\033[0m\n", #name); \
      code \
    } \
  } \

#define DSE_SUITE_TEST(name, code) \
  for(;;) { \
    if(dse_has_substring(#name, dse_query)) { \
      printf("\033[95m> %s\033[0m\n", #name); \
      code \
    } \
    break; \
  } \

#define DSE_TEST(name, code) \
  void dse_##name() { \
    if(dse_has_substring(#name, dse_query)) { \
      printf("\033[97m> %s\033[0m\n", #name); \
      code \
    } \
  } \

/// @todo: From this line to the end, think of better names.
typedef void (*dse_test_function)();
#define dse_max_test_functions 10000
dse_test_function dse_test_functions[dse_max_test_functions] = {0};
dse_u64 dse_functions_insert_index = 0;

dse_u64 dse_tests_per_thread = 1;
dse_u64 dse_remaining_tests = 0;

typedef struct {
  dse_u64 start_index;
  dse_u64 end_index;
} DSEThreadArgs;

dse_u64 dse_range_tests_proc(void* thread_args) {
  DSEThreadArgs args = *(DSEThreadArgs*)thread_args;
  for(dse_u64 i = args.start_index; i < args.end_index; i++) {
		dse_test_functions[i]();
	}
  return 0;
}