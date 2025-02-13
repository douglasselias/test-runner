#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

#include "file_reader.c"
#include "string_matcher.c"

void embed_file(FILE* file_descriptor, char* variable_name, File file) {
  fprintf(file_descriptor, "\n\nstatic unsigned char %s[] = {", variable_name);
  for(int i = 0; i < file.size - 1; i++) {
    fprintf(file_descriptor, "0x%x,", file.text[i]);
  }
  fprintf(file_descriptor, " 0x%x };", file.text[file.size-1]);
  fprintf(file_descriptor, "unsigned long %s_size = %zd;", variable_name, file.size);
}

int main() {
  FILE* generated_embed_file = fopen("lib_embed.c", "w");

  #ifdef _WIN64
  File os_file = read_entire_file("../os/dse_windows.c");
  #elif defined(__linux__)
  File os_file = read_entire_file("../os/dse_linux.c");
  #endif

  File assert_file  = read_entire_file("../dse_assert.c");

  File string_matcher_file = read_entire_file("../string_matcher.c");

  /// @note: If the variable name is changed, then it must change on main.c
  /// @note: This has a implicit order. string_matcher.c should come first.
  embed_file(generated_embed_file, "string_matcher_file", string_matcher_file);
  embed_file(generated_embed_file, "os_file", os_file);
  embed_file(generated_embed_file, "dse_assert_file", assert_file);

  fclose(generated_embed_file);
}