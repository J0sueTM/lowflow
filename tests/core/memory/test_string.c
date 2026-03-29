#include "../../../src/core/memory/string.h"
#include "../../../vendor/munit/munit.h"

static MunitResult test_init_string__success(const MunitParameter params[],
                                             void *data) {
  (void)params;
  (void)data;

  LF_Arena arena;
  LF_String str;
  str.arena = &arena;
  str.slice = NULL;
  str.char_qtt_in_block = 8;
  lf_init_string(&str);

  return MUNIT_OK;
}

static MunitResult test_init_string__slice(const MunitParameter params[],
                                           void *data) {
  (void)params;
  (void)data;

  LF_Arena arena;
  LF_ArenaSlice slice;
  LF_String str;
  str.arena = &arena;
  str.slice = &slice;
  lf_init_string(&str);

  munit_assert_size(slice.elem_size, ==, sizeof(char));

  return MUNIT_OK;
}

static MunitTest string_tests[] = {
  {
    .name = "/init",
    .test = test_init_string__success,
    .setup = NULL,
    .tear_down = NULL,
    .options = MUNIT_TEST_OPTION_NONE,
    .parameters = NULL,
  },
  {0},
};

static const MunitSuite string_test_suite = {
  .prefix = "/string",
  .tests = string_tests,
  .suites = NULL,
  .iterations = 1,
  .options = MUNIT_SUITE_OPTION_NONE,
};
