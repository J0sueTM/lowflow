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

#include "../src/hashdict.h"
#include "../libs/munit/munit.h"

static MunitResult test_hashdict_lifetime(
  const MunitParameter params[],
  void *data
) {
  (void)params;
  (void)data;

  HashDict *hd = hashdict_alloc(0, 0);
  munit_assert_int(
    hd->entries_arena->region_cap,
    ==,
    sizeof(HashDictEntry) * DEFAULT_ENTRY_CAP_IN_REGION
  );

  for (int i = 0; i < DEFAULT_HEAD_ENTRY_CAP; ++i) {
    // If not apparent, just checking if the head exists throught the
    // key.
    munit_assert_null((hd->heads + i)->key);
  }

  hashdict_free(hd);

  return MUNIT_OK;
}

static MunitResult test_hashdict_add_entry_colliding(
  const MunitParameter params[],
  void *data
) {
  (void)params;
  (void)data;

  HashDict *hd = hashdict_alloc(2, 2);

  char *key = "hello";
  char *val = "bye";
  hashdict_add_entry(hd, key, 5, val, 3);

  HashDictEntry *head_entry = (HashDictEntry *)hd->entries_arena->top->data;
  munit_assert_int(strcmp(head_entry->key, key), ==, 0);
  munit_assert_int(strcmp(head_entry->val, val), ==, 0);

  // Bruteforced a colliding key. Nothing special here.
  hashdict_add_entry(hd, "hello2", 6, "bye2", 4);
  munit_assert_int(strcmp(head_entry->next->key, "hello2"), ==, 0);
  munit_assert_int(strcmp(head_entry->next->val, "bye2"), ==, 0);

  hashdict_free(hd);

  return MUNIT_OK;
}

static MunitTest hashdict_tests[] = {
  { (char *)"/lifetime", test_hashdict_lifetime, 0, 0, 0, 0 },
  {
    (char *)"/add_entry/colliding",
    test_hashdict_add_entry_colliding,
    0, 0, 0, 0
  },
  {0}
};

static const MunitSuite hashdict_test_suite = {
  (char *)"/hashdict",
  hashdict_tests,
  0, 0
};
