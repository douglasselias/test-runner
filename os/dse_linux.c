#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdint.h>
#include<stdbool.h>
#include<ctype.h>
#include<sys/sysinfo.h>
#include<pthread.h>
#include<dirent.h>

typedef int64_t  dse_s64;
typedef uint64_t dse_u64;

/// @note: This is not platform dependent but I don't feel like moving this to another file :)
bool dse_has_substring(const char* haystack, const char* needle) {
  dse_u64 haystack_length = strlen(haystack);
  dse_u64 needle_length   = strlen(needle);

  if(needle_length == 0) return true;
  if(haystack_length < needle_length) return false;

  dse_u64 haystack_index = 0;
  dse_u64 needle_index   = 0;

  while(haystack_index < haystack_length) {
    if(tolower(haystack[haystack_index]) == tolower(needle[needle_index])) {
      needle_index++;

      if(needle_index < needle_length) haystack_index++;
      else return true;
    } else {
      needle_index = 0;
      haystack_index++;
    }
  }

  return false;
}

dse_u64 dse_count_threads() {
  return get_nprocs();
}

typedef pthread_t dse_thread_id;
typedef void(*dse_thread_proc)(void* args);

typedef struct {
  dse_thread_proc thread_proc;
  void* args;
} DSEThreadProcWrapperArgs;

void* dse_thread_proc_wrapper(void* args) {
  DSEThreadProcWrapperArgs wrapper_args = *(DSEThreadProcWrapperArgs*)args;
  wrapper_args.thread_proc(wrapper_args.args);
  return NULL;
}

dse_thread_id dse_create_thread(DSEThreadProcWrapperArgs* wrapper_args) {
  dse_thread_id* thread_id = calloc(sizeof(dse_thread_id), 1);
  pthread_create(thread_id, NULL, dse_thread_proc_wrapper, wrapper_args);
  return *thread_id;
}

void dse_wait_all_threads(dse_thread_id* thread_array, dse_u64 total_threads) {
  for(dse_u64 i = 0; i < total_threads; i++)
    pthread_join(thread_array[i], NULL);
}

void dse_atomic_increment(dse_s64* n) {
  __atomic_fetch_add(n, 1, __ATOMIC_SEQ_CST);
}

#define dse_max_filenames 1000
char* dse_list_of_filenames[dse_max_filenames] = {0};
dse_u64 dse_filename_insert_index = 0;

void dse_list_files_from_dir(const char* path) {
  DIR* d = opendir(path);
  if(d) {
    struct dirent* dir;
    while((dir = readdir(d)) != NULL) {
      char* dir_name = dir->d_name;
      unsigned char dir_type = dir->d_type;
      if(dir_name[0] != '.') {
        if(dir_type == DT_DIR) {
          printf("Recurse: %s\n", dir_name);
          char* full_path = calloc(sizeof(char), strlen(path) + strlen(dir_name));
          strcpy(full_path, path);
          strcat(full_path, "/");
          strcat(full_path, dir_name);
          dse_list_files_from_dir(full_path);
        } else if(dir_type == DT_REG) {
          if(dse_has_substring(dir_name, ".test.")) {
            printf("File: %s\n", dir_name);
            dse_list_of_filenames[dse_filename_insert_index] = calloc(sizeof(char), (strlen(path) + strlen(dir_name) + 2)); /// @todo: not sure if is needed here -> one slash between '..' and the filename, and the null terminator.

            strcat(dse_list_of_filenames[dse_filename_insert_index], path);
            strcat(dse_list_of_filenames[dse_filename_insert_index], "/");
            strcat(dse_list_of_filenames[dse_filename_insert_index], dir_name);
            dse_filename_insert_index++;
          }
        }
      }
    }
    closedir(d);
  }
}
