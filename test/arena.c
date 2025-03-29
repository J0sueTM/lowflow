#include "../src/arena.h"
#include "../libs/munit/munit.h"

static MunitResult test_arena_lifetime(
  const MunitParameter params[],
  void *data
) {
  (void)params;
  (void)data;

  Arena *arena = arena_alloc(0);
  munit_assert_not_null(arena->top);
  munit_assert_size(arena->region_cap, ==, DEFAULT_ARENA_CAP);

  arena_free(arena);

  return MUNIT_OK;
};

static MunitResult test_arena_malloc(
  const MunitParameter params[],
  void *data
) {
  (void)params;
  (void)data;

  Arena *arena = arena_alloc(6);

  arena_malloc(arena, 5);
  munit_assert_size(arena->top->used, ==, 6);
  munit_assert_size(arena->top->remainder, ==, 0);

  arena_malloc(arena, 5);
  munit_assert_not_null(arena->top->next);
  munit_assert_size(arena->top->next->used, ==, 6);
  munit_assert_size(arena->top->next->remainder, ==, 0);

  arena_free(arena);

  return MUNIT_OK;
}

static MunitResult test_arena_reset(
  const MunitParameter params[],
  void *data
) {
  (void)params;
  (void)data;

  Arena *arena = arena_alloc(0);

  arena_malloc(arena, 10);

  arena_reset(arena);
  munit_assert_size(arena->top->used, ==, 0);
  munit_assert_size(arena->top->remainder, ==, DEFAULT_ARENA_CAP);

  arena_free(arena);

  return MUNIT_OK;
}

static MunitTest arena_tests[] = {
  { (char *)"/lifetime", test_arena_lifetime, 0, 0, MUNIT_TEST_OPTION_NONE, 0 },
  { (char *)"/malloc", test_arena_malloc, 0, 0, MUNIT_TEST_OPTION_NONE, 0 },
  { (char *)"/reset", test_arena_reset, 0, 0, MUNIT_TEST_OPTION_NONE, 0 },
  {0}
};

static const MunitSuite arena_test_suite = {
  (char *)"",
  arena_tests,
  NULL,
  MUNIT_SUITE_OPTION_NONE
};
