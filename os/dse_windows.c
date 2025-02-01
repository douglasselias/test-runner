#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdint.h>
#include<stdbool.h>

#define WIN32_LEAN_AND_MEAN
#include<windows.h>

typedef int64_t  dse_s64;
typedef uint64_t dse_u64;

void dse_copy_string(const char* source, char* destination) {
  strcpy(destination, source);
}

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

void dse_execute_shell_cmd(char* cmd) {
  system(cmd);
}

dse_u64 dse_count_threads() {
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  return sysinfo.dwNumberOfProcessors;
}

typedef HANDLE dse_thread_id;
typedef void(*dse_thread_proc)(void* args);

typedef struct {
  dse_thread_proc thread_proc;
  void* args;
} ThreadProcWrapperArgs;

DWORD thread_proc_wrapper(void* args) {
  ThreadProcWrapperArgs wrapper_args = *(ThreadProcWrapperArgs*)args;
  wrapper_args.thread_proc(wrapper_args.args);
  return 0;
}

dse_thread_id dse_create_thread(ThreadProcWrapperArgs* wrapper_args) {
  LPSECURITY_ATTRIBUTES default_security_attr = NULL;
  dse_u64 default_stack_size = 0;
  return CreateThread(default_security_attr, default_stack_size, thread_proc_wrapper, wrapper_args, CREATE_SUSPENDED, NULL);
}

void dse_start_thread(dse_thread_id id) {
  ResumeThread(id);
}

void dse_wait_all_threads(dse_thread_id* thread_array, dse_u64 total_threads) {
  WaitForMultipleObjects((dse_u64)total_threads, thread_array, true, INFINITE);
}

void dse_atomic_increment(dse_s64* n) {
  InterlockedIncrement64(n);
}

#define dse_max_filenames 1000
char* dse_list_of_filenames[dse_max_filenames] = {0};
dse_u64 dse_filename_insert_index = 0;

void dse_list_files_from_dir(const char* path) {
  char dir_name[MAX_PATH] = {0};
  dse_copy_string(path, dir_name);
  strcat(dir_name, "\\*");

  WIN32_FIND_DATA ffd;
  HANDLE find_file_handle = FindFirstFile(dir_name, &ffd);

  if(INVALID_HANDLE_VALUE == find_file_handle) {
    puts("Invalid file or directory.");
    return;
  }

  do {
    if(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      if(ffd.cFileName[0] != '.') {
        char dir_buffer[MAX_PATH] = {0};
        dse_copy_string(path, dir_buffer);
        strcat(dir_buffer, "\\");
        strcat(dir_buffer, ffd.cFileName);
        dse_list_files_from_dir(dir_buffer);
      }
    } else {
      // LARGE_INTEGER filesize;
      // filesize.LowPart = ffd.nFileSizeLow;
      // filesize.HighPart = ffd.nFileSizeHigh;
      if(dse_has_substring(ffd.cFileName, ".test.")) {
        dse_list_of_filenames[dse_filename_insert_index] = calloc(sizeof(char), (strlen(path) + strlen(ffd.cFileName) + 2)); // one slash between '..' and the filename, and the null terminator.

        strcat(dse_list_of_filenames[dse_filename_insert_index], path);
        strcat(dse_list_of_filenames[dse_filename_insert_index], "/");
        strcat(dse_list_of_filenames[dse_filename_insert_index], ffd.cFileName);

        /// @note: Convert backwards slash to forward slash
        int i = 0;
        size_t filename_length = strlen(dse_list_of_filenames[dse_filename_insert_index]);
        while(i < filename_length) {
          if(dse_list_of_filenames[dse_filename_insert_index][i] == '\\') {
            dse_list_of_filenames[dse_filename_insert_index][i] = '/';
          }
          i++;
        }

        /// @todo: Maybe I can use this in the read file...
        // list_of_filesizes[dse_filename_insert_index] = filesize.QuadPart;
        printf("Found: %s\n", dse_list_of_filenames[dse_filename_insert_index]);
        dse_filename_insert_index++;
      }
    }
  } while(FindNextFile(find_file_handle, &ffd) != 0);
}
