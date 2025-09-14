#include "../src/arena.h"
#include "../vendor/munit/munit.h"
#include <stdalign.h>

static MunitResult test_init_arena(
  const MunitParameter params[],
  void *data
) {
  (void)params;
  (void)data;

  size_t block_size = 256;

  LF_Arena arena = { .block_size = block_size };
  lf_init_arena(&arena);

  munit_assert_ptr_not_null(arena.top_block);
  munit_assert_ptr_not_null(arena.top_block->data);
  munit_assert_size(arena.top_block->size, ==, block_size);

  return MUNIT_OK;
}

static MunitResult test_arena_alloc(
  const MunitParameter params[],
  void *data
) {
  (void)params;
  (void)data;

  size_t block_size = sizeof(char);

  LF_Arena arena = { .block_size = block_size };
  lf_init_arena(&arena);

  char *ptr = lf_arena_alloc(&arena, sizeof(char), alignof(char));
  munit_assert_ptr_not_null(ptr);
  munit_assert_size(arena.top_block->offset, ==, sizeof(char));

  return MUNIT_OK;
}

static MunitResult test_arena_alloc_overflow(
  const MunitParameter params[],
  void *data
) {
  (void)params;
  (void)data;

  size_t block_size = 2 * sizeof(char);

  LF_Arena arena = { .block_size = block_size };
  lf_init_arena(&arena);

  lf_arena_alloc(&arena, sizeof(char), alignof(char));
  char *snd_ptr = lf_arena_alloc(&arena, 2 * sizeof(char), alignof(char));

  munit_assert_size(arena.block_count, ==, 2);
  munit_assert_size(arena.head_block.offset, ==, sizeof(char));
  munit_assert_ptr_not_equal(&arena.head_block, arena.top_block);
  munit_assert_ptr_equal(arena.head_block.next, arena.top_block);

  return MUNIT_OK;
}

static MunitResult test_arena_alloc_alignment(
  const MunitParameter params[],
  void *data
) {
  (void)params;
  (void)data;

  size_t block_size = 16;

  LF_Arena arena = { .block_size = block_size };
  lf_init_arena(&arena);

  size_t fst_alloc_size = 3;
  lf_arena_alloc(&arena, fst_alloc_size, 0);
  char *aligned_ptr = lf_arena_alloc(&arena, sizeof(double), alignof(double));

  size_t expected_misalignment = 5;
  size_t expected_offset = fst_alloc_size + expected_misalignment + sizeof(double);
  munit_assert_size(arena.top_block->offset, ==, expected_offset);

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
  { "/alloc", test_arena_alloc, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { "/alloc/overflow", test_arena_alloc_overflow, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { "/alloc/alignment", test_arena_alloc_alignment, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { 0 }
};

static const MunitSuite arena_suite = {
  .prefix = "/arena",
  .tests = arena_tests,
  .suites = NULL,
  .iterations = 1,
  .options = MUNIT_SUITE_OPTION_NONE
};
