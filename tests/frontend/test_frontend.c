#include "../../vendor/munit/munit.h"
#include "./test_lexical_analysis.c"

static MunitSuite frontend_inner_test_suites[] = {
  lexical_analysis_test_suite,
  {0},
};

static const MunitSuite frontend_test_suite = {
  .prefix = "/frontend",
  .tests = NULL,
  .suites = frontend_inner_test_suites,
  .iterations = 1,
  .options = MUNIT_SUITE_OPTION_NONE,
};
