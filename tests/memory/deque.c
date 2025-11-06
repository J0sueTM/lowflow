#include <stdalign.h>

#include "../../src/memory/deque.h"
#include "../../vendor/munit/munit.h"

static MunitResult test_push_head__empty_head_block(
  const MunitParameter params[],
  void *data
) {
  (void)params;
  (void)data;

  LF_Deque deque = {
    .elem_size = sizeof(float),
    .elem_alignment = alignof(float),
    .elem_qtt_in_block = 3
  };
  lf_init_deque(&deque);

  float elem = *(float *)lf_alloc_deque_head_elem(&deque);

  munit_assert_size(deque.elem_count, ==, 1);
  munit_assert_size(deque.arena.block_count, ==, 2);
  munit_assert_ptr_equal(deque.arena.head_block->next, deque.arena.tail_block);
  munit_assert_size(deque.head_read_offset, ==, 2 * sizeof(float));

  return MUNIT_OK;
}

static MunitResult test_push_tail__empty_head_block(
  const MunitParameter params[],
  void *data
) {
  (void)params;
  (void)data;

  LF_Deque deque = {
    .elem_size = sizeof(float),
    .elem_alignment = alignof(float),
    .elem_qtt_in_block = 3
  };
  lf_init_deque(&deque);

  float elem = *(float *)lf_alloc_deque_tail_elem(&deque);

  munit_assert_size(deque.elem_count, ==, 1);
  munit_assert_size(deque.arena.block_count, ==, 1);
  munit_assert_ptr_equal(deque.arena.head_block, deque.arena.tail_block);
  munit_assert_size(deque.head_read_offset, ==, 0);

  return MUNIT_OK;
}

static MunitResult test_pop_head__single_elem(
  const MunitParameter params[],
  void *data
) {
  (void)params;
  (void)data;

  LF_Deque deque = {
    .elem_size = sizeof(float),
    .elem_alignment = alignof(float),
    .elem_qtt_in_block = 3
  };
  lf_init_deque(&deque);

  float *elem = (float *)lf_alloc_deque_head_elem(&deque);
  *elem = 7.f;

  float *popped_elem = (float *)lf_pop_head_elem_from_deque(&deque);

  munit_assert_ptr_not_null(popped_elem);
  munit_assert_ptr_equal(popped_elem, elem);
  munit_assert_float(*popped_elem, ==, *elem);
  munit_assert_size(deque.elem_count, ==, 0);

  return MUNIT_OK;
}

static MunitTest deque_tests[] = {
  {
    .name = "/push/head/empty_head_block",
    .test = test_push_head__empty_head_block,
    .setup = NULL,
    .tear_down = NULL,
    .options = MUNIT_TEST_OPTION_NONE,
    .parameters = NULL,
  },
  { "/push/tail/empty_head_block", test_push_tail__empty_head_block, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { "/pop/head/single_elem", test_pop_head__single_elem, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  {0},
};

static const MunitSuite deque_suite = {
  .prefix = "/memory/deque",
  .tests = deque_tests,
  .suites = NULL,
  .iterations = 1,
  .options = MUNIT_SUITE_OPTION_NONE,
};
