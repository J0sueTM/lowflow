#include "./arena.c"
#include "../libs/munit/munit.h"

static MunitSuite inner_suites[] = {
  arena_test_suite,
  {0}
};

static const MunitSuite test_suite = {
  (char *)"/arena",
  NULL,
  inner_suites,
  1,
  MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char *argv[MUNIT_ARRAY_PARAM(argc + 1)]) {
  return munit_suite_main(&test_suite, (void *)"lowflow", argc, argv);
}
