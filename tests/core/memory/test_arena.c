#include <stdalign.h>

#include "../../../src/core/memory/arena.h"
#include "../../../vendor/munit/munit.h"

static MunitResult test_init_arena(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  size_t float_padded_size = sizeof(float) + alignof(float);

  LF_Arena arena;
  arena.block_size = float_padded_size * 2;
  lf_init_arena(&arena);

  munit_assert_size(arena.block_count, ==, 1);

  lf_dealloc_arena(&arena);

  return MUNIT_OK;
}

static MunitResult test_arena_alloc_in_initial_block(
  const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  size_t float_padded_size = sizeof(float) + alignof(float);

  LF_Arena arena;
  arena.block_size = float_padded_size * 2;
  lf_init_arena(&arena);

  lf_arena_alloc_at_head(&arena, float_padded_size);
  lf_arena_alloc_at_tail(&arena, float_padded_size);

  munit_assert_size(arena.head_block->left_offset, ==, 0);
  munit_assert_size(arena.head_block->right_offset, ==, float_padded_size * 2);
  munit_assert_size(arena.block_count, ==, 1);

  lf_dealloc_arena(&arena);

  return MUNIT_OK;
}

static MunitResult test_arena_alloc_in_multiple_blocks(
  const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  size_t float_padded_size = sizeof(float) + alignof(float);

  LF_Arena arena;
  arena.block_size = float_padded_size * 2;
  lf_init_arena(&arena);

  lf_arena_alloc_at_tail(&arena, float_padded_size);
  lf_arena_alloc_at_tail(&arena, float_padded_size);

  // [_,>x<] - [>x<,_]
  munit_assert_size(arena.block_count, ==, 2);

  lf_arena_alloc_at_head(&arena, float_padded_size);
  lf_arena_alloc_at_head(&arena, float_padded_size);

  // [_,>x<] - [>x,x<] - [>x<,_]
  munit_assert_size(arena.block_count, ==, 3);

  lf_arena_alloc_at_head(&arena, float_padded_size);
  lf_arena_alloc_at_head(&arena, float_padded_size);

  // [_, >x<] - [>x,x<] - [>x,x<] - [>x<,_]
  munit_assert_size(arena.block_count, ==, 4);

  lf_arena_alloc_at_tail(&arena, float_padded_size);
  lf_arena_alloc_at_tail(&arena, float_padded_size);

  // [_, >x<] - [>x,x<] - [>x,x<] - [>x,x<], [>x<,_]
  munit_assert_size(arena.block_count, ==, 5);

  lf_dealloc_arena(&arena);

  return MUNIT_OK;
}

static MunitTest arena_tests[] = {{
                                    .name = "/init",
                                    .test = test_init_arena,
                                    .setup = NULL,
                                    .tear_down = NULL,
                                    .options = MUNIT_TEST_OPTION_NONE,
                                    .parameters = NULL,
                                  },
                                  {
                                    "/alloc/initial_block",
                                    test_arena_alloc_in_initial_block,
                                    NULL,
                                    NULL,
                                    MUNIT_TEST_OPTION_NONE,
                                    NULL,
                                  },
                                  {
                                    "/alloc/multiple_blocks",
                                    test_arena_alloc_in_multiple_blocks,
                                    NULL,
                                    NULL,
                                    MUNIT_TEST_OPTION_NONE,
                                    NULL,
                                  },
                                  {0}};

static const MunitSuite arena_test_suite = {
  .prefix = "/arena",
  .tests = arena_tests,
  .suites = NULL,
  .iterations = 1,
  .options = MUNIT_SUITE_OPTION_NONE,
};
