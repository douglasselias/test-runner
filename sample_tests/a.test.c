void test_some_test() {
  int array[] = {1, 2, 3};
  // DSE_ASSERT(array[1] == 1, "Oh no %d", array[1]);
  DSE_ASSERT(array[1] == 2);
  DSE_ASSERT(array[2] == 3);
  // DSE_ASSERT(array[0] == 3);
}

// void suite_string_joins() { // @only
//   // setup
//   openfile
//   { // skip things correctly @skip
//     // some explanation
//     int array[] = {1, 2, 3};
//     String8 a = STR8("");
//     String8 b = STR8("");
//     String8 c = STR8("");
//     DSE_ASSERT(array[1] == 2);
//     DSE_ASSERT(array[1] == 2);
//   } // @@skip

//   { // join strings
//     int array[] = {1, 2, 3};
//     String8 a = STR8("hello");
//     String8 b = STR8("sailor");
//     String8 c = STR8("ready?");
//     DSE_ASSERT(array[1] == 2);
//     DSE_ASSERT(array[1] == 2);
//   }
//   // teardown
//   closefile
// }