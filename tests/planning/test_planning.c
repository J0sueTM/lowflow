#include "../../vendor/munit/munit.h"
#include "./passes/test_passes.c"

static MunitSuite planning_inner_test_suites[] = {
  passes_test_suite,
  {0},
};

static const MunitSuite planning_test_suite = {
  .prefix = "/planning",
  .tests = NULL,
  .suites = planning_inner_test_suites,
  .iterations = 1,
  .options = MUNIT_SUITE_OPTION_NONE};
