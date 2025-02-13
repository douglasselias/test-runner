# Test runner for C projects

## This software is in early alpha!

This is a simple test runner that automatically finds your tests and leverages multithreading for executing them. The API is super simple, it only has ONE macro. The test runner is a single binary that weights less than 200kb.

## Quick API overview

> `assertion(boolean_expression, format_string, variable_arguments_for_string);`

## How to use

First, create a file containing `.test.` in the name, for example: `code.test.c`.

```c
#include "your_code.c"

void your_test() {
  int result = sum(3, 4);
  assertion(result == 7);

  // You can have more than one assertion per test.
  assertion(result > 0);

  // You can also provide a format string.
  assertion(result == 7, "Expected 7, but got %d", result);
}

// But what if I want a test suite?
// Just do this...
void your_suite() {
  // setup code
  FILE* file = fopen("a_file.txt");

  {
    bool result = file_has_newlines(file);
    assertion(result);
  }

  {
    bool result = file_has_execute_permissions(file);
    assertion(result);
  }

  // teardown code
  fclose(file);
}
```

Then execute the runner on your folder project. It will scan the entire directory tree to find tests.