#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define WIN32_LEAN_AND_MEAN
#include<windows.h>

uint32_t __count_threads() {
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  return sysinfo.dwNumberOfProcessors;
}

typedef HANDLE __thread_id;
typedef void(*__thread_proc_t)(void* args);

typedef struct {
  __thread_proc_t thread_proc;
  void* args;
} __ThreadProcWrapperArgs;

DWORD __thread_proc_wrapper(void* args) {
  __ThreadProcWrapperArgs wrapper_args = *(__ThreadProcWrapperArgs*)args;
  wrapper_args.thread_proc(wrapper_args.args);
  return 0;
}

__thread_id __create_thread(__ThreadProcWrapperArgs* wrapper_args) {
  return CreateThread(NULL, 0, __thread_proc_wrapper, wrapper_args, 0, NULL);
}

void __wait_all_threads(__thread_id* thread_array, uint64_t total_threads) {
  WaitForMultipleObjects((DWORD)total_threads, thread_array, true, INFINITE);
}

void __atomic_increment(uint64_t* n) {
  InterlockedIncrement64((int64_t*)n);
}

#define __max_filenames 1000
char* __list_of_filenames[__max_filenames] = {0};
uint64_t __filename_insert_index = 0;

void __list_files_from_dir(const char* path) {
  char dir_name[MAX_PATH] = {0};
  strcpy(dir_name, path);
  strcat(dir_name, "\\*");

  WIN32_FIND_DATA ffd;
  HANDLE find_file_handle = FindFirstFile(dir_name, &ffd);

  if(INVALID_HANDLE_VALUE == find_file_handle) return;

  do {
    if(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      if(ffd.cFileName[0] != '.') {
        char dir_buffer[MAX_PATH] = {0};
        strcpy(dir_buffer, path);
        strcat(dir_buffer, "\\");
        strcat(dir_buffer, ffd.cFileName);
        __list_files_from_dir(dir_buffer);
      }
    } else {
      if(__has_substring(ffd.cFileName, ".test.")) {
        __list_of_filenames[__filename_insert_index] = calloc(sizeof(char), (strlen(path) + strlen("/") + strlen(ffd.cFileName) + strlen(" \0")));

        strcat(__list_of_filenames[__filename_insert_index], path);
        strcat(__list_of_filenames[__filename_insert_index], "/");
        strcat(__list_of_filenames[__filename_insert_index], ffd.cFileName);

        /// @note: Convert backwards slash to forward slash
        uint64_t i = 0;
        size_t filename_length = strlen(__list_of_filenames[__filename_insert_index]);
        while(i < filename_length) {
          if(__list_of_filenames[__filename_insert_index][i] == '\\') {
            __list_of_filenames[__filename_insert_index][i] = '/';
          }
          i++;
        }

        __filename_insert_index++;
      }
    }
  } while(FindNextFile(find_file_handle, &ffd) != 0);
}
