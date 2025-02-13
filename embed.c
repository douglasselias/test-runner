#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

typedef int64_t  dse_s64;
typedef uint64_t dse_u64;

/// @todo: CopyPasta from dse_windows.c
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

typedef struct {
  char* text;
  size_t size;
} File;

File read_entire_file(const char* path) {
  FILE* file_descriptor = fopen(path, "r");
  File file = {"", 0};

  if(file_descriptor == NULL) {
    printf("File %s not found.\n", path);
    return file;
  }

  fseek(file_descriptor, 0, SEEK_END);
  size_t size = ftell(file_descriptor);
  fseek(file_descriptor, 0, SEEK_SET);

  char* string = calloc(sizeof(char), size + 1);
  fread(string, size, 1, file_descriptor);
  fclose(file_descriptor);

  string[size] = '\0';
  
  file.text = string;
  file.size = size;
  return file;
}

void embed_file(FILE* file_descriptor, char* variable_name, File file) {
  fprintf(file_descriptor, "\n\nstatic unsigned char %s[] = {", variable_name);
  for(int i = 0; i < file.size - 1; i++) {
    fprintf(file_descriptor, "0x%x,", file.text[i]);
  }
  fprintf(file_descriptor, " 0x%x };", file.text[file.size-1]);
}

void remove_os_include(char* string) {
  char* line = strtok(string, "\n");
  dse_u64 i = 0;
  while(line != NULL) {
    dse_u64 line_size = strlen(line);
    /// @note: This is the file name in the os folder.
    if(dse_has_substring(line, "dse_windows")
    || dse_has_substring(line, "dse_linux")) {
      dse_u64 line_end = i + line_size + 1;
      for(; i < line_end; i++) {
        string[i] = ' ';
      }
      break;
    }
    i += line_size;
    line = strtok(NULL, "\n");
  }
}

int main() {
  FILE* generated_embed_file = fopen("lib_embed.c", "w");

  #ifdef _WIN64
  File os_file = read_entire_file("../os/dse_windows.c");
  #elif defined(__linux__)
  File os_file = read_entire_file("../os/dse_linux.c");
  #endif
  File dse_assert_file  = read_entire_file("../dse_assert.c");
  remove_os_include(dse_assert_file.text);

  /// @note: If the variable name is changed, then it must change on main.c
  embed_file(generated_embed_file, "os_file", os_file);
  embed_file(generated_embed_file, "dse_assert_file", dse_assert_file);
  fclose(generated_embed_file);
}