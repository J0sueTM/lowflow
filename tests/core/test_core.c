#include "../../vendor/munit/munit.h"
#include "./memory/test_memory.c"

static MunitSuite core_inner_test_suites[] = {
  memory_test_suite,
  {0},
};

static const MunitSuite core_test_suite = {
  .prefix = "/core",
  .tests = NULL,
  .suites = core_inner_test_suites,
  .iterations = 1,
  .options = MUNIT_SUITE_OPTION_NONE,
};
