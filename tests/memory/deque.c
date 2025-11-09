#include <stdalign.h>

#include "../../src/memory/deque.h"
#include "../../vendor/munit/munit.h"

static MunitResult test_push_head_empty_head_block(
  const MunitParameter params[],
  void *data
) {
  (void)params;
  (void)data;

  LF_Deque deque = {
    .elem_size = sizeof(double),
    .elem_alignment = alignof(double),
    .elem_qtt_in_block = 3
  };
  lf_init_deque(&deque);

  lf_alloc_deque_head_elem(&deque);

  munit_assert_size(deque.elem_count, ==, 1);
  munit_assert_size(deque.arena.block_count, ==, 1);
  munit_assert_ptr_equal(deque.arena.head_block, deque.arena.tail_block);
  munit_assert_size(deque.head_read_offset, ==, 2 * sizeof(double));

  return MUNIT_OK;
}

static MunitResult test_push_tail_empty_head_block(
  const MunitParameter params[],
  void *data
) {
  (void)params;
  (void)data;

  LF_Deque deque = {
    .elem_size = sizeof(double),
    .elem_alignment = alignof(double),
    .elem_qtt_in_block = 3
  };
  lf_init_deque(&deque);

  lf_alloc_deque_tail_elem(&deque);

  munit_assert_size(deque.elem_count, ==, 1);
  munit_assert_size(deque.arena.block_count, ==, 2);
  munit_assert_ptr_equal(deque.arena.head_block->next, deque.arena.tail_block);
  munit_assert_size(deque.head_read_offset, ==, 3 * sizeof(double));

  return MUNIT_OK;
}

static MunitResult test_pop_head_single_elem(
  const MunitParameter params[],
  void *data
) {
  (void)params;
  (void)data;

  LF_Deque deque = {
    .elem_size = sizeof(double),
    .elem_alignment = alignof(double),
    .elem_qtt_in_block = 3
  };
  lf_init_deque(&deque);

  double *elem = (double *)lf_alloc_deque_head_elem(&deque);
  *elem = 7.f;

  double *popped_elem = (double *)lf_pop_head_elem_from_deque(&deque);

  munit_assert_ptr_not_null(popped_elem);
  munit_assert_ptr_equal(popped_elem, elem);
  munit_assert_double(*popped_elem, ==, *elem);
  munit_assert_size(deque.elem_count, ==, 0);

  return MUNIT_OK;
}

static MunitResult test_pop_head_multiple_blocks(
  const MunitParameter params[],
  void *data
) {
  (void)params;
  (void)data;

  LF_Deque deque = {
    .elem_size = sizeof(double),
    .elem_alignment = alignof(double),
    .elem_qtt_in_block = 1
  };
  lf_init_deque(&deque);

  lf_alloc_deque_head_elem(&deque);
  lf_alloc_deque_head_elem(&deque);
  lf_alloc_deque_head_elem(&deque);
  // [a] <--> [b] <--> [c]

  lf_pop_head_elem_from_deque(&deque);
  // [_] <--> [b] <--> [c]

  // Shouldn't have freed the head block yet.
  munit_assert_size(deque.arena.block_count, ==, 3);
  munit_assert_size(deque.elem_count, ==, 2);

  lf_pop_head_elem_from_deque(&deque);
  lf_pop_head_elem_from_deque(&deque);
  // NULL - NULL <- []

  munit_assert_size(deque.arena.block_count, ==, 1);
  munit_assert_size(deque.elem_count, ==, 0);

  return MUNIT_OK;
}

static MunitResult test_pop_tail_multiple_blocks(
  const MunitParameter params[],
  void *data
) {
  (void)params;
  (void)data;

  LF_Deque deque = {
    .elem_size = sizeof(double),
    .elem_alignment = alignof(double),
    .elem_qtt_in_block = 1
  };
  lf_init_deque(&deque);
  
  lf_alloc_deque_tail_elem(&deque);
  lf_alloc_deque_tail_elem(&deque);
  lf_alloc_deque_tail_elem(&deque);
  // [] <--> [a] <--> [b] <--> [c]

  lf_pop_tail_elem_from_deque(&deque);
  // [] <--> [a] <--> [b] <--> []

  // Shouldn't have freed the tail block yet.
  munit_assert_size(deque.arena.block_count, ==, 4);
  munit_assert_size(deque.elem_count, ==, 2);

  lf_pop_tail_elem_from_deque(&deque);
  // [] <--> [a] <--> [] -> NULL
  lf_pop_tail_elem_from_deque(&deque);
  // [] <--> [] <--> NULL - NULL

  munit_assert_size(deque.arena.block_count, ==, 2);
  munit_assert_size(deque.elem_count, ==, 0);

  return MUNIT_OK;
}

static MunitResult test_pop_tail_single_elem(
  const MunitParameter params[],
  void *data
) {
  (void)params;
  (void)data;

  LF_Deque deque = {
    .elem_size = sizeof(double),
    .elem_alignment = alignof(double),
    .elem_qtt_in_block = 3
  };
  lf_init_deque(&deque);

  double *elem = (double *)lf_alloc_deque_tail_elem(&deque);
  *elem = 7.f;

  double *popped_elem = (double *)lf_pop_tail_elem_from_deque(&deque);

  munit_assert_ptr_not_null(popped_elem);
  munit_assert_ptr_equal(popped_elem, elem);
  munit_assert_double(*popped_elem, ==, *elem);
  munit_assert_size(deque.elem_count, ==, 0);

  return MUNIT_OK;
}

static MunitResult test_deque_grows_when_tail_is_pushed_after_head_full(
  const MunitParameter params[],
  void *data
) {
  (void)params; (void)data;

  LF_Deque deque = {
    .elem_size = sizeof(double),
    .elem_alignment = alignof(double),
    .elem_qtt_in_block = 3
  };
  lf_init_deque(&deque);

  lf_alloc_deque_head_elem(&deque);
  lf_alloc_deque_head_elem(&deque);
  lf_alloc_deque_head_elem(&deque);

  munit_assert_size(deque.arena.block_count, ==, 1);

  lf_alloc_deque_tail_elem(&deque);

  munit_assert_size(deque.arena.block_count, ==, 2);

  return MUNIT_OK;
}

static MunitResult test_deque_frees_blocks_when_emptied(
  const MunitParameter params[],
  void *data
) {
  (void)params; (void)data;

  LF_Deque deque = {
    .elem_size = sizeof(double),
    .elem_alignment = alignof(double),
    .elem_qtt_in_block = 3
  };
  lf_init_deque(&deque);

  lf_alloc_deque_head_elem(&deque);
  lf_alloc_deque_tail_elem(&deque);
  lf_alloc_deque_tail_elem(&deque);

  // Should have allocated 2 blocks.
  munit_assert_size(deque.arena.block_count, ==, 2);

  lf_pop_head_elem_from_deque(&deque);
  lf_pop_head_elem_from_deque(&deque);
  lf_pop_head_elem_from_deque(&deque);

  // Should be reduced to one reusable block.
  munit_assert_size(deque.arena.block_count, ==, 1);

  return MUNIT_OK;
}

static MunitResult test_deque_reuses_existing_blocks(
  const MunitParameter params[],
  void *data
) {
  (void)params; (void)data;

  LF_Deque deque = {
    .elem_size = sizeof(double),
    .elem_alignment = alignof(double),
    .elem_qtt_in_block = 3
  };
  lf_init_deque(&deque);

  for (int i = 0; i < 6; i++) {
    lf_alloc_deque_head_elem(&deque);
  }
  munit_assert_size(deque.arena.block_count, ==, 2);

  while (deque.elem_count > 0) {
    lf_pop_head_elem_from_deque(&deque);
  }
  munit_assert_size(deque.arena.block_count, ==, 1);

  lf_alloc_deque_head_elem(&deque);
  lf_alloc_deque_head_elem(&deque);
  munit_assert_size(deque.arena.block_count, ==, 1);

  return MUNIT_OK;
}

static MunitResult test_deque_mixed_operations_preserve_order(
  const MunitParameter params[], void *data
) {
  (void)params; (void)data;

  LF_Deque deque = {
    .elem_size = sizeof(double),
    .elem_alignment = alignof(double),
    .elem_qtt_in_block = 3
  };
  lf_init_deque(&deque);

  double *a = (double *)lf_alloc_deque_head_elem(&deque); *a = 7;
  double *b = (double *)lf_alloc_deque_head_elem(&deque); *b = 11;
  double *tail = (double *)lf_alloc_deque_tail_elem(&deque); *tail = 29;

  double *pop1 = (double *)lf_pop_head_elem_from_deque(&deque);
  double *pop2 = (double *)lf_pop_tail_elem_from_deque(&deque);

  munit_assert_double(*pop1, ==, 11);
  munit_assert_double(*pop2, ==, 29);
  munit_assert_size(deque.elem_count, ==, 1);

  return MUNIT_OK;
}

static MunitResult test_deque_complex_growth_and_release(
  const MunitParameter params[], void *data
) {
  (void)params; (void)data;

  LF_Deque deque = {
    .elem_size = sizeof(double),
    .elem_alignment = alignof(double),
    .elem_qtt_in_block = 3
  };
  lf_init_deque(&deque);

  for (int i = 0; i < 9; i++) {
    double *e = (double *)lf_alloc_deque_head_elem(&deque);
    *e = (double)i;
  }
  munit_assert_size(deque.arena.block_count, ==, 3);

  lf_pop_head_elem_from_deque(&deque);
  lf_pop_tail_elem_from_deque(&deque);

  // Should still keep 3 blocks since middle one is active.
  munit_assert_size(deque.arena.block_count, ==, 3);

  // Pop until only one block is left.
  while (deque.elem_count > 1) {
    lf_pop_head_elem_from_deque(&deque);
  }
  munit_assert_size(deque.arena.block_count, ==, 1);

  return MUNIT_OK;
}

static MunitResult test_integration(
  const MunitParameter params[],
  void *data
) {
  (void)params;
  (void)data;

  LF_Deque deque = {
    .elem_size = sizeof(double),
    .elem_alignment = alignof(double),
    .elem_qtt_in_block = 3
  };
  lf_init_deque(&deque);

  // > = head read offset
  // < = block write offset
  //
  // [__,__,__><] -> NULL
  double *elem = (double *)lf_alloc_deque_head_elem(&deque);
  *elem = 7.f;
  // [__,__,>7.f<] -> NULL

  elem = (double *)lf_alloc_deque_head_elem(&deque);
  *elem = 11.f;
  // [__,>11.f,7.f<] -> NULL

  elem = (double *)lf_pop_tail_elem_from_deque(&deque);
  munit_assert_double(*elem, ==, 7.f);
  // [__,>11.f<,__] -> NULL

  elem = (double *)lf_alloc_deque_tail_elem(&deque);
  *elem = 29.f;
  // [__,>11.f,29.f<] -> NULL

  elem = (double *)lf_alloc_deque_tail_elem(&deque);
  *elem = 52.f;
  // [__,>11.f,29.f<] <--> [52.f<,__,__]

  elem = (double *)lf_pop_head_elem_from_deque(&deque);
  munit_assert_double(*elem, ==, 11.f);
  // [__,__,>29.f<] <--> [52.f<,__,__]

  elem = (double *)lf_pop_head_elem_from_deque(&deque);
  munit_assert_double(*elem, ==, 29.f);
  // [__,__,__><] <--> [52.f<,__,__]

  elem = (double *)lf_pop_head_elem_from_deque(&deque);
  munit_assert_double(*elem, ==, 52.f);
  // NULL <- [,__,__><]

  munit_assert_size(deque.arena.block_count, ==, 1);
  
  elem = (double *)lf_alloc_deque_head_elem(&deque);
  *elem = 77.f;
  // [__,__,>77.f<]

  munit_assert_size(deque.arena.block_count, ==, 1);

  elem = (double *)lf_alloc_deque_head_elem(&deque);
  *elem = 12.f;
  // [__,>12.f,77.f<]

  elem = (double *)lf_alloc_deque_head_elem(&deque);
  *elem = 49.f;
  // [>49.f,12.f,77.f<]

  munit_assert_size(deque.arena.block_count, ==, 1);
  
  elem = (double *)lf_alloc_deque_tail_elem(&deque);
  *elem = 33.f;
  // [>49.f,12.f,77.f<] <--> [33.f<,__,__]

  munit_assert_size(deque.arena.block_count, ==, 2);

  lf_pop_head_elem_from_deque(&deque);
  // [__,>12.f,77.f<] <--> [33.f<,__,__]
  lf_pop_head_elem_from_deque(&deque);
  // [__,__,>77.f<] <--> [33.f<,__,__]

  elem = (double *)lf_alloc_deque_head_elem(&deque);
  *elem = 2.3f;
  // [__,>2.3f,77.f<] <--> [33.f<,__,__]

  elem = (double *)lf_alloc_deque_head_elem(&deque);
  *elem = 19.f;
  // [>19.f,2.3f,77.f<] <--> [33.f<,__,__]

  elem = (double *)lf_alloc_deque_head_elem(&deque);
  *elem = 27.f;
  // [__,__,>27.f<] <--> [19.f,2.3f,77.f<] <--> [33.f<,__,__]

  munit_assert_size(deque.arena.block_count, ==, 3);

  lf_pop_head_elem_from_deque(&deque);
  lf_pop_tail_elem_from_deque(&deque);
  // [__,__,__><] <--> [19.f,2.3f,77.f<] <--> [<__,__,__]

  munit_assert_size(deque.arena.block_count, ==, 3);
  
  elem = (double *)lf_pop_head_elem_from_deque(&deque);
  munit_assert_double(*elem, ==, 19.f);
  // NULL <- [__,>2.3f,77.f<] <--> [<_,__,__]

  munit_assert_size(deque.arena.block_count, ==, 2);

  lf_pop_tail_elem_from_deque(&deque);
  // NULL <- [__,>2.3f<,__] -> NULL

  munit_assert_size(deque.arena.block_count, ==, 1);
  munit_assert_size(deque.elem_count, ==, 1);

  return MUNIT_OK;
}

static MunitTest deque_tests[] = {
  {
    .name = "/push/head/empty_head_block",
    .test = test_push_head_empty_head_block,
    .setup = NULL,
    .tear_down = NULL,
    .options = MUNIT_TEST_OPTION_NONE,
    .parameters = NULL,
  },
  { "/push/tail/empty_head_block", test_push_tail_empty_head_block, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { "/pop/head/single_elem", test_pop_head_single_elem, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { "/pop/head/multiple_blocks", test_pop_head_multiple_blocks, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { "/pop/tail/single_elem", test_pop_tail_single_elem, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { "/pop/tail/multiple_blocks", test_pop_tail_multiple_blocks, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { "/grow/tail_after_head_full", test_deque_grows_when_tail_is_pushed_after_head_full, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { "/free/blocks_when_emptied", test_deque_frees_blocks_when_emptied, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { "/reuse/blocks", test_deque_reuses_existing_blocks, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { "/mixed_operations/preserve_order", test_deque_mixed_operations_preserve_order, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { "/complex_growth/release", test_deque_complex_growth_and_release, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { "/integration", test_integration, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  {0},
};

static const MunitSuite deque_suite = {
  .prefix = "/memory/deque",
  .tests = deque_tests,
  .suites = NULL,
  .iterations = 1,
  .options = MUNIT_SUITE_OPTION_NONE,
};
