DSE_SUITE(a_suite,
  DSE_SUITE_TEST(a_test,
    DSE_ASSERT(*dse_total_tests == 4, "Got %lld\n", *dse_total_tests);
  );
);

// DSE_SUITE(a_suite,
//   DSE_ASSERT(*dse_total_tests == 100, "nah");
//   DSE_ASSERT(*dse_total_tests == 0, "Oh no");
//   DSE_SKIP(DSE_ASSERT(1 == 1));
// );

// DSE_TEST(another_test,
//   DSE_ASSERT(*dse_total_tests == 5);
// );