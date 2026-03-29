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

  LF_List list = {
    .elem_size = sizeof(double),
    .elem_alignment = alignof(double),
    .elem_qtt_in_block = 3,
  };
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

static MunitResult test_init_arena_slice__success(const MunitParameter params[],
                                                  void *data) {
  (void)params;
  (void)data;

  size_t float_padded_size = sizeof(float) + alignof(float);

  LF_Arena arena;
  arena.block_offset = LF_MEMBLOCK_OFFSET_LEFT;
  arena.block_size = float_padded_size * 2;
  lf_init_arena(&arena);

  lf_arena_alloc_at_tail(&arena, float_padded_size * 2);
  lf_arena_alloc_at_tail(&arena, float_padded_size * 2);
  lf_arena_alloc_at_tail(&arena, float_padded_size * 2);
  // [x,x] -> [x,x] -> [x,x]

  LF_ArenaSlice slice;
  slice.elem_size = float_padded_size;
  lf_init_arena_slice(&slice, &arena);

  munit_assert_size(slice.head_offset, ==, arena.head_block->left_offset);
  munit_assert_size(slice.tail_offset, ==, arena.tail_block->right_offset);

  lf_dealloc_arena(&arena);

  return MUNIT_OK;
}

static MunitResult test_arena_slice_alloc_at_head__success(
  const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  size_t float_padded_size = sizeof(float) + alignof(float);

  LF_Arena arena;
  arena.block_offset = LF_MEMBLOCK_OFFSET_MIDDLE;
  arena.block_size = float_padded_size * 2;
  lf_init_arena(&arena);

  LF_ArenaSlice slice;
  slice.elem_size = float_padded_size;
  lf_init_arena_slice(&slice, &arena);

  char *ptr = lf_arena_slice_alloc_at_head(&slice, &arena, float_padded_size);
  munit_assert_not_null(ptr);
  munit_assert_ptr_equal(slice.head_block, arena.head_block);
  munit_assert_size(slice.head_offset, ==, arena.head_block->left_offset);

  // Will create a new memblock.
  ptr = lf_arena_slice_alloc_at_head(&slice, &arena, float_padded_size);
  munit_assert_not_null(ptr);
  munit_assert_ptr_equal(slice.head_block, arena.head_block);
  munit_assert_size(slice.head_offset, ==, arena.head_block->left_offset);

  lf_dealloc_arena(&arena);

  return MUNIT_OK;
}

static MunitResult test_arena_slice_alloc_at_tail__success(
  const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  size_t float_padded_size = sizeof(float) + alignof(float);

  LF_Arena arena;
  arena.block_offset = LF_MEMBLOCK_OFFSET_MIDDLE;
  arena.block_size = float_padded_size * 2;
  lf_init_arena(&arena);

  LF_ArenaSlice slice;
  slice.elem_size = float_padded_size;
  lf_init_arena_slice(&slice, &arena);

  char *ptr = lf_arena_slice_alloc_at_tail(&slice, &arena, float_padded_size);
  munit_assert_not_null(ptr);
  munit_assert_ptr_equal(slice.tail_block, arena.tail_block);
  munit_assert_size(slice.tail_offset, ==, arena.tail_block->right_offset);

  // Will create a new memblock.
  ptr = lf_arena_slice_alloc_at_tail(&slice, &arena, float_padded_size);
  munit_assert_not_null(ptr);
  munit_assert_ptr_equal(slice.tail_block, arena.tail_block);
  munit_assert_size(slice.tail_offset, ==, arena.tail_block->right_offset);

  lf_dealloc_arena(&arena);

  return MUNIT_OK;
}

static MunitResult test_arena_slice_alloc__both_sides(
  const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  size_t float_padded_size = sizeof(float) + alignof(float);

  LF_Arena arena;
  arena.block_offset = LF_MEMBLOCK_OFFSET_LEFT;
  arena.block_size = float_padded_size * 2;
  lf_init_arena(&arena);

  LF_ArenaSlice slice;
  slice.elem_size = float_padded_size;
  lf_init_arena_slice(&slice, &arena);

  char *head_ptr =
    lf_arena_slice_alloc_at_head(&slice, &arena, float_padded_size * 2);
  char *tail_ptr =
    lf_arena_slice_alloc_at_tail(&slice, &arena, float_padded_size * 2);

  munit_assert_not_null(head_ptr);
  munit_assert_not_null(tail_ptr);
  munit_assert_ptr_not_equal(head_ptr, tail_ptr);

  lf_dealloc_arena(&arena);

  return MUNIT_OK;
}

static MunitResult test_arena_slice__ownership(const MunitParameter params[],
                                               void *data) {
  (void)params;
  (void)data;

  size_t float_padded_size = sizeof(float) + alignof(float);

  LF_Arena arena;
  arena.block_offset = LF_MEMBLOCK_OFFSET_MIDDLE;
  arena.block_size = float_padded_size * 2;
  lf_init_arena(&arena);

  LF_ArenaSlice slice;
  slice.elem_size = float_padded_size;
  lf_init_arena_slice(&slice, &arena);

  // Inits owning the whole arena.
  char *head_ptr =
    lf_arena_slice_alloc_at_head(&slice, &arena, float_padded_size);
  // [x,_]
  char *tail_ptr =
    lf_arena_slice_alloc_at_tail(&slice, &arena, float_padded_size);
  // [x,x]
  munit_assert_ptr_not_null(head_ptr);
  munit_assert_ptr_not_null(tail_ptr);

  // Loses head.
  lf_arena_alloc_at_head(&arena, float_padded_size);
  // [_,y] -> [x,x]
  head_ptr = lf_arena_slice_alloc_at_head(&slice, &arena, float_padded_size);
  munit_assert_ptr_null(head_ptr);

  // Loses tail.
  lf_arena_alloc_at_tail(&arena, float_padded_size);
  // [_,y] -> [x,x] - [y,_]
  tail_ptr = lf_arena_slice_alloc_at_tail(&slice, &arena, float_padded_size);
  munit_assert_ptr_null(head_ptr);

  lf_dealloc_arena(&arena);

  return MUNIT_OK;
}

static MunitResult test_arena_slice__multiple(const MunitParameter params[],
                                              void *data) {
  (void)params;
  (void)data;

  size_t float_padded_size = sizeof(float) + alignof(float);

  LF_Arena arena;
  arena.block_size = float_padded_size * 2;
  arena.block_offset = LF_MEMBLOCK_OFFSET_LEFT;
  lf_init_arena(&arena);

  LF_ArenaSlice fst_slice;
  fst_slice.elem_size = float_padded_size;

  // Slice 1 inits owning the whole arena.
  lf_init_arena_slice(&fst_slice, &arena);
  // []
  lf_arena_slice_alloc_at_tail(&fst_slice, &arena, float_padded_size * 2);
  // [1,1]
  lf_arena_slice_alloc_at_tail(&fst_slice, &arena, float_padded_size * 2);
  // [1,1] -> [1,1]
  lf_arena_slice_alloc_at_tail(&fst_slice, &arena, float_padded_size * 2);
  // [1,1] -> [1,1] -> [1,1]

  // Slice 2 inits owning the second block.
  LF_ArenaSlice snd_slice = {
    .head_block = arena.head_block->next,
    .tail_block = arena.head_block->next,
    .elem_size = float_padded_size,
  };
  // [1,1] -> [(1 2),(1 2)] -> [1,1]

  // Since it isn't in the borders, it can't grow.
  char *head_ptr =
    lf_arena_slice_alloc_at_head(&snd_slice, &arena, float_padded_size);
  char *tail_ptr =
    lf_arena_slice_alloc_at_tail(&snd_slice, &arena, float_padded_size);
  munit_assert_ptr_null(head_ptr);
  munit_assert_ptr_null(tail_ptr);

  // Slice 1, however, can grow, since it's still on the borders.
  tail_ptr =
    lf_arena_slice_alloc_at_tail(&fst_slice, &arena, float_padded_size * 2);
  // [1, 1] -> [(1 2),(1 2)] -> [1,1] -> [1,1]
  munit_assert_ptr_not_null(tail_ptr);

  // Slice 3 inits owning the last (fourth) block.
  LF_ArenaSlice thd_slice = {
    .head_block = arena.tail_block,
    .tail_block = arena.tail_block,
    .head_offset = arena.tail_block->left_offset,
    .tail_offset = arena.tail_block->right_offset,
  };
  // [1,1] -> [(1 2),(1 2)] -> [1,1] -> [(1 3),(1 3)]
  tail_ptr =
    lf_arena_slice_alloc_at_tail(&thd_slice, &arena, float_padded_size);
  munit_assert_ptr_not_null(tail_ptr);

  // Now that slice 3 grew, slice 1 can't grow anymore at the tail.
  tail_ptr =
    lf_arena_slice_alloc_at_tail(&fst_slice, &arena, float_padded_size);
  munit_assert_ptr_null(tail_ptr);

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
  {
    "/slice/init",
    test_init_arena_slice__success,
    NULL,
    NULL,
    MUNIT_TEST_OPTION_NONE,
    NULL,
  },
  {
    "/slice/alloc/head",
    test_arena_slice_alloc_at_head__success,
    NULL,
    NULL,
    MUNIT_TEST_OPTION_NONE,
    NULL,
  },
  {
    "/slice/alloc/tail",
    test_arena_slice_alloc_at_tail__success,
    NULL,
    NULL,
    MUNIT_TEST_OPTION_NONE,
    NULL,
  },
  {
    "/slice/alloc/both",
    test_arena_slice_alloc__both_sides,
    NULL,
    NULL,
    MUNIT_TEST_OPTION_NONE,
    NULL,
  },
  {
    "/slice/ownership",
    test_arena_slice__ownership,
    NULL,
    NULL,
    MUNIT_TEST_OPTION_NONE,
    NULL,
  },
  {
    "/slice/ownership/multiple",
    test_arena_slice__multiple,
    NULL,
    NULL,
    MUNIT_TEST_OPTION_NONE,
    NULL,
  },
  {0},
};

static const MunitSuite adapters_test_suite = {
  .prefix = "/adapters",
  .tests = adapters_tests,
  .suites = NULL,
  .iterations = 1,
  .options = MUNIT_SUITE_OPTION_NONE,
};
