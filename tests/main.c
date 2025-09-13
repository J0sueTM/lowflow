#include "./arena.c"

static MunitSuite inner_suites[] = {
  arena_suite,
  { 0 }
};

static const MunitSuite main_suite = {
  .prefix = "lowflow",
  .tests = NULL,
  .suites = inner_suites,
  .iterations = 1,
  .options = MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char *argv[MUNIT_ARRAY_PARAM(argc + 1)]) {
  return munit_suite_main(&main_suite, (void *)"lowflow", argc, argv);
}
