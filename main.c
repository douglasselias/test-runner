#include "dse_assert.c"

DSE_SUITE(a_suite,
  DSE_ASSERT(*dse_total_tests == 100, "nah");
  DSE_ASSERT(*dse_total_tests == 0, "Oh no");
  DSE_SKIP(DSE_ASSERT(1 == 1));

  DSE_SUITE_TEST(a_test,
    DSE_ASSERT(*dse_total_tests == 4);
  );
);

DSE_TEST(another_test,
  DSE_ASSERT(*dse_total_tests == 5);
);

int main() {
  puts("Hello Sailor");
  dse_init_results();

  dse_a_suite();
  dse_another_test();

  dse_print_results();
}