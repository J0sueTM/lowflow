#include <stdalign.h>

#include "../../../src/core/memory/list.h"
#include "../../../vendor/munit/munit.h"

static MunitResult test_init_list(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  LF_List list;
  list.elem_size = sizeof(float);
  list.elem_alignment = alignof(float);
  list.elem_qtt_in_block = 2;
  lf_init_list(&list);

  return MUNIT_OK;
}

static MunitTest list_tests[] = {{
                                   .name = "/init",
                                   .test = test_init_list,
                                   .setup = NULL,
                                   .tear_down = NULL,
                                   .options = MUNIT_TEST_OPTION_NONE,
                                   .parameters = NULL,
                                 },
                                 {0}};

static const MunitSuite list_test_suite = {
  .prefix = "/list",
  .tests = list_tests,
  .suites = NULL,
  .iterations = 1,
  .options = MUNIT_SUITE_OPTION_NONE,
};
