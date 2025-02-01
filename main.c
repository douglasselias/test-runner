#include "os/dse_windows.c"

dse_s64* counter;

void thread_proc(void* args) {
  int id = *(int*)args;
  printf("Inc from Thread ID: %d, before:\t%lld\n", id, *counter);
  dse_atomic_increment(counter);
  printf("Inc from Thread ID: %d, after:\t%lld\n", id, *counter);
}

int main() {
  puts("Hello Sailor");

  counter = calloc(sizeof(dse_s64), 1);
  *counter = 0;

  #define total_threads 9
  dse_thread_id threads[total_threads] = {0};

  for(int i = 0; i < total_threads; i++) {
    int* id = calloc(sizeof(int), 1);
    *id = i;
    ThreadProcWrapperArgs* args = calloc(sizeof(ThreadProcWrapperArgs), 1);
    args->thread_proc = thread_proc;
    args->args = id;
    threads[i] = dse_create_thread(args);
  }

  for(int i = 0; i < total_threads; i++) {
    dse_start_thread(threads[i]);
  }

  dse_wait_all_threads(threads, total_threads);

  printf("Total Counter\t%lld\n", *counter);
}