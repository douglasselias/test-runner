# Test runner for C projects

This is a simple test runner that automatically finds your tests and leverages multithreading for executing them. The API is super simple, it only has five macros. The test runner is a single binary that weights less than 200kb.

## Quick API overview

> `DSE_TEST(valid_c_identifier, code);` Creates an test.

> `DSE_ASSERT(boolean_expression, format_string, variable_arguments_for_string);`

> `DSE_SKIP(DSE_ASSERT());` Can only wrap the assert macro.

> `DSE_SUITE(valid_c_identifier, code);` Creates an isolated group of tests.

> `DSE_SUITE_TEST(valid_c_identifier, code);` Creates an test (only valid for declaring inside a suite)

## How to use

First, create a file containing `.test.` in the name, for example: `code.test.c`. Now you can write a test or a suite.

```c
#include "your_code.c"

// A single test.
DSE_TEST(your_test,
  int result = sum(3, 4);
  DSE_ASSERT(result == 7);

  // You can have more than one assertion per test.
  DSE_ASSERT(result > 0);

  // You can also provide a format string.
  DSE_ASSERT(result == 7, "Expected 7, but got %d", result);

  // You can skip an assert. The assertions are the ones that count as tests.
  DSE_SKIP(DSE_ASSERT(1 == 1));
);

// A suite is a good way to group related tests, specially if they share some common code for startup and teardown.
DSE_SUITE(your_suite,
  some_startup_code();

  DSE_SUITE_TEST(your_test_inside_a_suite,
    int result = sum(3, 4);
    DSE_ASSERT(result == 7);
  );

  some_teardown_code();
);
```

Then execute the runner on your folder project. It will scan the entire directory tree to find tests.