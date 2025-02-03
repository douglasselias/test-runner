#include "dse_assert.c"

#define RELEASE 1
#ifdef RELEASE
#include "build/lib_embed.c"
#endif

bool strings_are_equal(char* a, char* b) {
  return strcmp(a, b) == 0;
}

char* read_entire_file(const char* path) {
  FILE* file = fopen(path, "rb");

  if(file == NULL) {
    printf("File %s not found.\n", path);
    return "";
  }

  /// @todo: I could use a struct with path and size. I only need to confirm if by reading the file system on Linux I can get the file size. Then I could remove these three lines.
  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  // printf("SIZE: %zd\n", size);
  fseek(file, 0, SEEK_SET);

  char* string = calloc(sizeof(char), size + 1);
  fread(string, size, 1, file);
  fclose(file);

  string[size] = '\0';
  return string;
}

dse_s64 char_index(const char* haystack, char needle) {
  for(dse_u64 i = 0; i < strlen(haystack); i++) {
    if(haystack[i] == needle) return i;
  }
  return -1;
}

char* test_names[dse_max_test_functions] = {0};
dse_u64 test_names_insert_index = 0;

void extract_name_of_test(char* text) {
  dse_u64 test_name_length = 200;
  char* test_name = calloc(sizeof(char), test_name_length);
  char* line = strtok(text, "\n");

  while(line != NULL) {
    /// @todo: Not a robust way to detect commented lines.
    bool is_not_a_commented_line = !dse_has_substring(line, "//");
    bool is_a_test = dse_has_substring(line, "DSE_SUITE(")
                  || dse_has_substring(line, "DSE_TEST(");

    if(is_not_a_commented_line && is_a_test) {
      dse_u64 parenthesis_index = char_index(line, '(') + 1;
      dse_u64 coma_index = char_index(line, ',');

      test_names[test_names_insert_index] = calloc(sizeof(char), test_name_length);
      memcpy(test_names[test_names_insert_index], line + parenthesis_index, coma_index - parenthesis_index);
      test_names_insert_index++;
    }

    line = strtok(NULL, "\n");
  }
}

dse_s64 main(dse_u64 argc, char* argv[]) {
  char* cli_arg = argv[1] != NULL && strlen(argv[1]) > 0 ? argv[1] : "";

  if(strings_are_equal("help", cli_arg)
  || strings_are_equal("-help", cli_arg)
  || strings_are_equal("--help", cli_arg)) {
    puts("This software is in early alpha!");
    // puts("\nYou can provide a query to execute all the tests that contains the query.");
    // puts("\t> test_runner.exe my_query");
    // puts("Running without a query will execute all tests.");
    puts("Tests and Suites names must be a valid C identifier.");
    puts("Give a star! https://github.com/douglasselias/test-runner");
    return 0;
  } else {
    /// @todo: Copy the string to dse_query.
  }

  printf("\nThis system has %lld processors\n\n", dse_count_threads());
  char* separator = "----------------------------------------";

  puts(separator);
  puts("::: Searching test files :::");
  /// @todo: Maybe a better API should be to return the list of directories.
  #ifdef RELEASE
    dse_list_files_from_dir(".");
    /// @note: Uncomment the line below to test in release mode.
    // dse_list_files_from_dir("..");
  #else
    dse_list_files_from_dir("..");
  #endif
  puts("::: Finished searching :::");
  puts(separator);

  puts("::: Generating file :::");
  dse_execute_shell_cmd("mkdir build");
  FILE* generated_file = fopen("build/generated.c", "w");

  #ifdef RELEASE
    dse_u64 windows_file_size = sizeof(dse_windows_file) / sizeof(dse_windows_file[0]);
    dse_u64 assert_file_size = sizeof(dse_assert_file) / sizeof(dse_assert_file[0]);
    for(dse_u64 i = 0; i < windows_file_size; i++) {
      fprintf(generated_file, "%c", dse_windows_file[i] == '\0' ? '\n' : (char)dse_windows_file[i]);
    }
    for(dse_u64 i = 0; i < assert_file_size; i++) {
      fprintf(generated_file, "%c", dse_assert_file[i] == '\0' ? '\n' : (char)dse_assert_file[i]);
    }
  #else
    fprintf(generated_file, "#include \"../dse_assert.c\"\n");
  #endif


  for(dse_u64 i = 0; i < dse_filename_insert_index; i++) {
    if(dse_list_of_filenames[i]) {
      #ifdef RELEASE
      fprintf(generated_file, "#include \"../%s\"\n", dse_list_of_filenames[i]);
      #else
      fprintf(generated_file, "#include \"%s\"\n", dse_list_of_filenames[i]);
      #endif
      char* file_text = read_entire_file(dse_list_of_filenames[i]);
      extract_name_of_test(file_text);
      /// @todo: A better approach is to allocate once and reuse.
      free(file_text);
    }
  }

  fprintf(generated_file, "\nint main() {");
  /// @todo: Copy the query to the generated file.
  // fprintf(generated_file, "\n\tdse_copy_string(\"%s\", dse_test_query);\n", test_query);

  for(dse_u64 i = 0; i < test_names_insert_index; i++) {
    if(test_names[i]) {
      fprintf(generated_file, "\n\tdse_test_functions[dse_functions_insert_index++] = dse_%s;", test_names[i]);
    }
  }

  fprintf(generated_file, "\n\n\tdse_init_results();");
  fprintf(generated_file, "\n\tdse_init_threads();");
  fprintf(generated_file, "\n\tdse_print_results();");

  fprintf(generated_file, "\n\n\treturn 0;\n}");

  fclose(generated_file);
  puts("::: Finished generating file :::");
  puts(separator);

  puts("::: Compiling :::");
  /// @todo: Give an option to the user specify the compiler command.
  dse_execute_shell_cmd("cl /nologo /diagnostics:caret /Wall /WX /W4 /wd4189 /wd4464 /wd5045 /wd4255 /wd4996 /wd4100 /wd4244 /Fo:\"build/generated\" build/generated.c /link /out:build/generated.exe");
  puts("::: Finished compiling :::");
  puts(separator);

  puts("::: Running tests :::");
  dse_execute_shell_cmd("build\\generated.exe");
  puts("::: Finished running tests :::");
}