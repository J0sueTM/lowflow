#include "../src/arena.h"
#include "../vendor/munit/munit.h"

static MunitResult test_init_arena(
  const MunitParameter params[],
  void *data
) {
  (void)params;
  (void)data;

  return MUNIT_OK;
}

static MunitTest arena_tests[] = {
  {
    .name = "/init",
    .test = test_init_arena,
    .setup = NULL,
    .tear_down = NULL,
    .options = MUNIT_TEST_OPTION_NONE,
    .parameters = NULL
  },
  { 0 }
};

static const MunitSuite arena_suite = {
  .prefix = "/arena",
  .tests = arena_tests,
  .suites = NULL,
  .iterations = 1,
  .options = MUNIT_SUITE_OPTION_NONE
};
