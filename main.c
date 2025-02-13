// #include "dse_assert.c"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "file_reader.c"
#include "string_matcher.c"
#include "os/dse_windows.c"

#include "build/lib_embed.c"

bool strings_are_equal(char* a, char* b) {
  return strcmp(a, b) == 0;
}

int64_t char_index(const char* haystack, char needle) {
  for(uint64_t i = 0; i < strlen(haystack); i++) {
    if(haystack[i] == needle) return i;
  }
  return -1;
}

/// @note: This [1000] is the same as dse_mas_test_functions
char* test_names[1000] = {0};
uint64_t test_names_insert_index = 0;

void extract_name_of_test(char* text) {
  uint64_t test_name_length = 200;
  char* test_name = calloc(sizeof(char), test_name_length);
  char* line = strtok(text, "\n");

  while(line != NULL) {
    /// @todo: Not a robust way to detect commented lines.
    /// Does not catch multiline comments!
    bool is_not_a_commented_line = !dse_has_substring(line, "//");
    // bool is_not_a_skip_assertion = !dse_has_substring(line, "DSE_SKIP(");
    bool is_a_test = dse_has_substring(line, "void")
                  && dse_has_substring(line, "()")
                  && dse_has_substring(line, "{");

    if(is_not_a_commented_line && is_a_test) {
      uint64_t test_name_start_index = char_index(line, 'd') + 2;
      uint64_t left_parenthesis_index = char_index(line, '(');

      test_names[test_names_insert_index] = calloc(sizeof(char), test_name_length);
      memcpy(test_names[test_names_insert_index], line + test_name_start_index, left_parenthesis_index - test_name_start_index);
      test_names_insert_index++;
    }

    line = strtok(NULL, "\n");
  }
}

int32_t main(uint64_t argc, char* argv[]) {
  char* cli_arg = argv[1] != NULL && strlen(argv[1]) > 0 ? argv[1] : "";

  if(strings_are_equal("help", cli_arg)) {
    puts("This software is in early alpha!");
    // puts("\nYou can provide a query to execute all the tests that contains the query.");
    // puts("\t> test_runner my_query");
    // puts("Running without a query will execute all tests.");
    puts("Tests names must be a valid C identifier.");
    puts("Give a star! https://github.com/douglasselias/test-runner");
    return 0;
  } else {
    /// @todo: Copy the string to dse_query.
  }

  printf("\nThis system has %d processors\n\n", dse_count_threads());
  char* separator = "----------------------------------------";

  puts(separator);
  puts("::: Searching test files :::");
  /// @todo: Maybe a better API should be to return the list of files.
  #define RELEASE 0
  #if RELEASE == 1
    dse_list_files_from_dir(".");
  #else
    dse_list_files_from_dir("..");
  #endif

  puts(separator);
  puts("::: Generating file :::");

  #ifdef _WIN64
  system("mkdir build");
  #elif defined(__linux__)
  system("mkdir -p build");
  #endif

  FILE* generated_file = fopen("build/generated.c", "w");

  int64_t string_matcher_size = sizeof(string_matcher_file) / sizeof(string_matcher_file[0]);
  for(int64_t i = 0; i < string_matcher_size; i++) {
    fprintf(generated_file, "%c", string_matcher_file[i] == '\0' ? '\n' : (char)string_matcher_file[i]);
  }

  int64_t os_file_size = sizeof(os_file) / sizeof(os_file[0]);
  for(int64_t i = 0; i < os_file_size; i++) {
    fprintf(generated_file, "%c", os_file[i] == '\0' ? '\n' : (char)os_file[i]);
  }

  int64_t assert_file_size = sizeof(dse_assert_file) / sizeof(dse_assert_file[0]);
  for(int64_t i = 0; i < assert_file_size; i++) {
    fprintf(generated_file, "%c", dse_assert_file[i] == '\0' ? '\n' : (char)dse_assert_file[i]);
  }

  for(uint64_t i = 0; i < dse_filename_insert_index; i++) {
    if(dse_list_of_filenames[i]) {
      fprintf(generated_file, "#include \"../%s\"\n", dse_list_of_filenames[i]);
      char* file_text = read_entire_file(dse_list_of_filenames[i]).text;
      extract_name_of_test(file_text);
      free(file_text);
    }
  }

  fprintf(generated_file, "\nint main() {");

  for(uint64_t i = 0; i < test_names_insert_index; i++) {
    if(test_names[i]) {
      fprintf(generated_file, "\n\tdse_test_functions[dse_functions_insert_index++] = %s;", test_names[i]);
    }
  }

  fprintf(generated_file, "\n\n\tdse_init_results();");
  fprintf(generated_file, "\n\tdse_run_threads();");
  fprintf(generated_file, "\n\tdse_print_results();");

  fprintf(generated_file, "\n\n\treturn 0;\n}");

  fclose(generated_file);

  system("cls");
  puts(separator);
  puts("::: Compiling :::");

  /// @todo: Give an option to the user specify the compiler command.
  #ifdef _WIN64
  int64_t exit_code = system("cl /nologo /diagnostics:caret /Z7 /fsanitize=address /Wall /WX /W4 /wd4189 /wd4464 /wd5045 /wd4255 /wd4996 /wd4100 /wd4244 /Fo:\"build/generated\" build/generated.c /link /pdbaltpath:build/generated.pdb /out:build/generated.exe");
  #elif defined(__linux__)
  int64_t exit_code = system("gcc -Wall -Wextra build/generated.c -o build/generated");
  #endif

  if(exit_code != 0) return -1;

  puts("");
  puts(separator);
  puts("::: Running tests :::");

  #ifdef _WIN64
  system("build\\generated");
  #elif defined(__linux__)
  system("build/generated");
  #endif
}