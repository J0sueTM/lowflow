/*
 * Copyright (C) Josué Teodoro Moreira
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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

  // |----------|
  Arena *arena = arena_alloc(10);

  // |*****|-----|
  arena_malloc(arena, 5);
  munit_assert_size(arena->top->used, ==, 5);
  munit_assert_size(arena->top->remainder, ==, 5);
  munit_assert_char(*(arena->top->data + 5), ==, (char)0);

  // |*****|***|--|
  arena_malloc(arena, 3);
  munit_assert_size(arena->top->used, ==, 8);
  munit_assert_size(arena->top->remainder, ==, 2);
  munit_assert_char(*(arena->top->data + 9), ==, (char)0);

  // |**********|   -> New top region.
  // |*****|***|--| -> Second region, used to be the top.
  arena_malloc(arena, 10);
  munit_assert_not_null(arena->top->next);
  munit_assert_size(arena->top->used, ==, 10);
  munit_assert_size(arena->top->remainder, ==, 0);

  // Make sure we skipped straight ahead to the next region.
  munit_assert_char(*(arena->top->next->data + 9), ==, (char)0);

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
  (char *)"/arena",
  arena_tests,
  0, 0
};
