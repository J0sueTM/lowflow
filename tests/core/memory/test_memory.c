#include "../../../vendor/munit/munit.h"
#include "./test_arena.c"
#include "./test_list.c"

static MunitSuite memory_inner_test_suites[] = {
  arena_test_suite,
  list_test_suite,
  {0},
};

static const MunitSuite memory_test_suite = {
  .prefix = "/memory",
  .tests = NULL,
  .suites = memory_inner_test_suites,
  .iterations = 1,
  .options = MUNIT_SUITE_OPTION_NONE,
};
