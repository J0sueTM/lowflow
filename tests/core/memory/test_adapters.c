#include <stdalign.h>

#include "../../../src/core/memory/adapters.h"
#include "../../../vendor/munit/munit.h"

static MunitResult test_stack_to_list__same_block_qtt(
  const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  LF_Stack stack = {
    .elem_size = sizeof(double),
    .elem_alignment = alignof(double),
    .elem_qtt_in_block = 3,
  };
  lf_init_stack(&stack);

  double *bottom_stack_elem = (double *)lf_alloc_stack_elem(&stack);
  lf_alloc_stack_elem(&stack);
  lf_alloc_stack_elem(&stack);
  lf_alloc_stack_elem(&stack);
  double *top_stack_elem = (double *)lf_alloc_stack_elem(&stack);

  *bottom_stack_elem = 4;
  *top_stack_elem = 5;

  LF_List list = {.elem_size = sizeof(double),
                  .elem_alignment = alignof(double),
                  .elem_qtt_in_block = 3};
  lf_init_list(&list);

  lf_alloc_arena_tail_block(&list.arena);

  lf_stack_to_list(&stack, &list);

  double *fst_list_elem = (double *)lf_get_first_list_elem(&list);
  double *last_list_elem = (double *)lf_get_last_list_elem(&list);

  munit_assert_double(*fst_list_elem, ==, *top_stack_elem);
  munit_assert_double(*last_list_elem, ==, *bottom_stack_elem);

  return MUNIT_OK;
}

static MunitResult test_stack_to_list__diff_block_qtt(
  const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  LF_Stack stack = {
    .elem_size = sizeof(double),
    .elem_alignment = alignof(double),
    .elem_qtt_in_block = 2,
  };
  lf_init_stack(&stack);

  double *bottom_stack_elem = (double *)lf_alloc_stack_elem(&stack);
  lf_alloc_stack_elem(&stack);

  lf_alloc_stack_elem(&stack);
  lf_alloc_stack_elem(&stack);

  lf_alloc_stack_elem(&stack);
  double *top_stack_elem = (double *)lf_alloc_stack_elem(&stack);

  *bottom_stack_elem = 4;
  *top_stack_elem = 5;

  LF_List list = {.elem_size = sizeof(double),
                  .elem_alignment = alignof(double),
                  .elem_qtt_in_block = 2};
  lf_init_list(&list);

  lf_stack_to_list(&stack, &list);

  double *fst_list_elem = (double *)lf_get_first_list_elem(&list);
  double *last_list_elem = (double *)lf_get_last_list_elem(&list);

  munit_assert_double(*fst_list_elem, ==, *top_stack_elem);
  munit_assert_double(*last_list_elem, ==, *bottom_stack_elem);
  munit_assert_size(list.arena.block_count, ==, stack.arena.block_count);

  return MUNIT_OK;
}

static MunitTest adapters_tests[] = {
  {
    .name = "/stack_to_list/same_block_qtt",
    .test = test_stack_to_list__same_block_qtt,
    .setup = NULL,
    .tear_down = NULL,
    .options = MUNIT_TEST_OPTION_NONE,
    .parameters = NULL,
  },
  {
    "/stack_to_list/diff_block_qtt",
    test_stack_to_list__diff_block_qtt,
    NULL,
    NULL,
    MUNIT_TEST_OPTION_NONE,
    NULL,
  },
  {0}};

static const MunitSuite adapters_test_suite = {
  .prefix = "/adapters",
  .tests = adapters_tests,
  .suites = NULL,
  .iterations = 1,
  .options = MUNIT_SUITE_OPTION_NONE,
};
