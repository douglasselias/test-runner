#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

uint64_t* __total_assertions_failed;

void __init_results() {
  __total_assertions_failed = calloc(sizeof(uint64_t), 1);
}

void __print_results() {
  if(*__total_assertions_failed == 0) {
    puts("\n\033[36mALL TESTS PASSED\033[0m");
  } else puts("");
}

/// @note: This is to remove the unnecessary full path of the assertion report.
uint64_t __reverse_index(char* string) {
  uint64_t length = strlen(string);
  bool found_first_dot = false;
  for(uint64_t i = length - 1; i > 0; i--) {
    if(found_first_dot && string[i] == '.') {
      return i+1;
    } else found_first_dot = false;
    if(string[i] == '.') {
      found_first_dot = true;
    }
  }

  return 0;
}

#define assertion(expression, ...) \
  if(!(expression)) { \
    __atomic_increment(__total_assertions_failed); \
    printf("\n\033[31mFAILED\033[0m\t%s:%d ", __FILE__ + __reverse_index(__FILE__), __LINE__); \
    printf("" __VA_ARGS__); \
  } \

typedef void (*__test_proc_t)();
#define __max_test_functions 10000
__test_proc_t __test_functions[__max_test_functions] = {0};
uint64_t __functions_insert_index = 0;

uint64_t __tests_per_thread  = 1;
uint64_t __remaining_tests   = 0;
uint64_t __available_threads = 1;

typedef struct {
  uint64_t __start_index;
  uint64_t end_index;
} __ThreadArgs;

void __range_tests_proc(void* thread_args) {
  __ThreadArgs args = *(__ThreadArgs*)thread_args;
  for(uint64_t i = args.__start_index; i < args.end_index; i++) {
		__test_functions[i]();
	}
}

void __run_threads() {
  __available_threads = __count_threads() - 1;

  if(__functions_insert_index >= __available_threads) {
    __tests_per_thread = __functions_insert_index / __available_threads;
    __remaining_tests  = __functions_insert_index % __available_threads;
  } else {
    __available_threads = 1;
    __tests_per_thread = __functions_insert_index;
  }

  __thread_id* __threads_array = calloc(sizeof(__thread_id), __available_threads);
  uint64_t __start_index = 0;

  for(uint64_t i = 0; i < __available_threads; i++) {
    __ThreadArgs* args = calloc(sizeof(__ThreadArgs), 1);
    args->__start_index = __start_index;
    args->end_index   = __start_index + __tests_per_thread + (i < __remaining_tests ? 1 : 0);
    __start_index = args->end_index;

    __ThreadProcWrapperArgs* wrapper_args = calloc(sizeof(__ThreadProcWrapperArgs), 1);
    wrapper_args->thread_proc = __range_tests_proc;
    wrapper_args->args = (void*)args;
    __threads_array[i] = __create_thread(wrapper_args);
  }

  __wait_all_threads(__threads_array, __available_threads);
}
