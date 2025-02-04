#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

typedef int64_t  dse_s64;
typedef uint64_t dse_u64;

/// @note: CopyPasta from dse_windows.c
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

  /// @todo: Not a robust way to remove the os/dse_windows.c include for embedding the source file. A better wayt is to remove any include that references a source file in this project, since it will be copy pasted on the final file to embed on the test runner binary.
  char* line = strtok(string, "\n");
  /// @todo: Change to support linux systems.
  if(dse_has_substring(line, "dse_windows")) {
    dse_u64 line_size = strlen(line);
    for(dse_u64 i = 0; i < line_size; i++) {
      string[i] = ' ';
    }
  }
  
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

int main() {
  FILE* generated_embed_file = fopen("lib_embed.c", "w");

  /// @todo: Change to support linux systems.
  File dse_windows_file = read_entire_file("../os/dse_windows.c");
  File dse_assert_file  = read_entire_file("../dse_assert.c");

  /// @todo: If the variable name is changed, then it must change on main.c
  embed_file(generated_embed_file, "dse_windows_file", dse_windows_file);
  embed_file(generated_embed_file, "dse_assert_file", dse_assert_file);
  fclose(generated_embed_file);
  puts("Finished embedding");
}