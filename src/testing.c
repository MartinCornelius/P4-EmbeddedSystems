#define MUNIT_ENABLE_ASSERT_ALIASES
#include "./lib/munit.h"


MunitResult my_test(const MunitParameter params[], void* user_data_or_fixture) {
  assert_int(2, ==, 2);
  return MUNIT_OK;
}

static MunitTest test_suite_tests[] = {
  { (char*) "type2text", my_test, MUNIT_TEST_OPTION_NONE, NULL },
};


static const MunitSuite test_suite = {
  (char*) "",
  test_suite_tests,
  NULL,
  1,
  MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)]) {
  return munit_suite_main(&test_suite, (void*) "µnit", argc, argv);
}