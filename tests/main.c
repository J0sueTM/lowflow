#include "../vendor/munit/munit.h"
#include "./core/test_core.c"

static MunitSuite main_inner_suites[] = {
  core_test_suite,
  { 0 }
};

static const MunitSuite main_test_suite = {
  .prefix = "lowflow",
  .tests = NULL,
  .suites = main_inner_suites,
  .iterations = 1,
  .options = MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char *argv[MUNIT_ARRAY_PARAM(argc + 1)]) {
  return munit_suite_main(&main_test_suite, (void *)"lowflow", argc, argv);
}
