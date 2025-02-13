#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/sysinfo.h>
#include <pthread.h>
#include <dirent.h>

uint8_t __count_threads() {
  return get_nprocs();
}

typedef pthread_t __thread_id;
typedef void(*__thread_proc_t)(void* args);

typedef struct {
  __thread_proc_t thread_proc;
  void* args;
} __ThreadProcWrapperArgs;

void* __thread_proc_wrapper(void* args) {
  __ThreadProcWrapperArgs wrapper_args = *(__ThreadProcWrapperArgs*)args;
  wrapper_args.thread_proc(wrapper_args.args);
  return NULL;
}

__thread_id __create_thread(__ThreadProcWrapperArgs* wrapper_args) {
  __thread_id* thread_id = calloc(sizeof(__thread_id), 1);
  pthread_create(thread_id, NULL, __thread_proc_wrapper, wrapper_args);
  return *thread_id;
}

void __wait_all_threads(__thread_id* thread_array, uint8_t total_threads) {
  for(uint8_t i = 0; i < total_threads; i++)
    pthread_join(thread_array[i], NULL);
}

void __atomic_increment(int64_t* n) {
  __atomic_fetch_add(n, 1, __ATOMIC_SEQ_CST);
}

#define __max_filenames 1000
char* __list_of_filenames[__max_filenames] = {0};
uint64_t __filename_insert_index = 0;

void __list_files_from_dir(const char* path) {
  DIR* d = opendir(path);
  if(d) {
    struct dirent* dir;
    while((dir = readdir(d)) != NULL) {
      char* dir_name = dir->d_name;
      unsigned char dir_type = dir->d_type;
      if(dir_name[0] != '.') {
        if(dir_type == DT_DIR) {
          char* full_path = calloc(sizeof(char), strlen(path) + strlen(dir_name));
          strcpy(full_path, path);
          strcat(full_path, "/");
          strcat(full_path, dir_name);
          __list_files_from_dir(full_path);
        } else if(dir_type == DT_REG) {
          if(__has_substring(dir_name, ".test.")) {
            printf("File: %s\n", dir_name);
            __list_of_filenames[__filename_insert_index] = calloc(sizeof(char), (strlen(path) + strlen(dir_name) + 2)); /// @todo: not sure if is needed here -> one slash between '..' and the filename, and the null terminator.

            strcat(__list_of_filenames[__filename_insert_index], path);
            strcat(__list_of_filenames[__filename_insert_index], "/");
            strcat(__list_of_filenames[__filename_insert_index], dir_name);
            __filename_insert_index++;
          }
        }
      }
    }
    closedir(d);
  }
}
