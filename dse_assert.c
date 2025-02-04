#include "os/dse_windows.c"
/// @todo: Change to support linux systems.

/// @todo: Should I rename to assertions?
dse_s64* dse_total_tests;
dse_s64* dse_total_tests_failed;

void dse_init_results() {
  dse_total_tests         = calloc(sizeof(dse_u64), 1);
  dse_total_tests_failed  = calloc(sizeof(dse_u64), 1);

  *dse_total_tests         = 0;
  *dse_total_tests_failed  = 0;
}

void dse_print_results() {
  /// @todo: Should I rename 'total tests' to assertions?
  dse_u64 total_tests_passed = *dse_total_tests - *dse_total_tests_failed;
  printf(
    "\n"
    "Total tests:\t%lld\n"
    "Total failed:\t%lld\n"
    "Total passed:\t%lld\n"
    ,
    *dse_total_tests,
    *dse_total_tests_failed,
    total_tests_passed
  );
}

#define DSE_ASSERT(expression, ...) \
  dse_atomic_increment(dse_total_tests); \
  if(!(expression)) { \
    dse_atomic_increment(dse_total_tests_failed); \
    printf("\033[31mFAILED\033[0m\t"); \
    printf("Line: %s:%d  ", __FILE__, __LINE__); \
    printf("" __VA_ARGS__); \
    puts(""); \
  } \

typedef void (*dse_test_function)();
#define dse_max_test_functions 10000
dse_test_function dse_test_functions[dse_max_test_functions] = {0};
dse_u64 dse_functions_insert_index = 0;

dse_u64 dse_tests_per_thread  = 1;
dse_u64 dse_remaining_tests   = 0;
dse_u64 dse_available_threads = 1;

typedef struct {
  dse_u64 start_index;
  dse_u64 end_index;
} DSEThreadArgs;

void dse_range_tests_proc(void* thread_args) {
  DSEThreadArgs args = *(DSEThreadArgs*)thread_args;
  for(dse_u64 i = args.start_index; i < args.end_index; i++) {
		dse_test_functions[i]();
	}
}

void dse_run_threads() {
  dse_available_threads = dse_count_threads() - 1;

  if(dse_functions_insert_index >= dse_available_threads) {
    dse_tests_per_thread = dse_functions_insert_index / dse_available_threads;
    dse_remaining_tests  = dse_functions_insert_index % dse_available_threads;
  } else {
    dse_available_threads = 1;
    dse_tests_per_thread = dse_functions_insert_index;
  }

  puts("");
  printf("Running %lld test(s) on %lld thread(s)\n", dse_functions_insert_index, dse_available_threads);
  printf("Tests per thread:  %lld\n", dse_tests_per_thread);
  printf("Remaining tests:   %lld\n", dse_remaining_tests);
  puts("");

  dse_thread_id* threads_array = calloc(sizeof(dse_thread_id), dse_available_threads);
  /// @todo: Maybe use two for loops, one to create the threads and the other to run the threads.
  dse_u64 start_index = 0;
  for(dse_u64 i = 0; i < dse_available_threads; i++) {
    DSEThreadArgs* args = calloc(sizeof(DSEThreadArgs), 1);
    args->start_index = start_index;
    args->end_index   = start_index + dse_tests_per_thread + (i < dse_remaining_tests ? 1 : 0);
    start_index = args->end_index;

    /// @todo: Do I really need another calloc?
    DSEThreadProcWrapperArgs* wrapper_args = calloc(sizeof(DSEThreadProcWrapperArgs), 1);
    wrapper_args->thread_proc = dse_range_tests_proc;
    wrapper_args->args = (void*)args;
    threads_array[i] = dse_create_thread(wrapper_args);
  }

  dse_wait_all_threads(threads_array, dse_available_threads);
}
