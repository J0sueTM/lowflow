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

#include "./hashdict.h"

HashDict *hashdict_alloc(
  Arena *arena,
  size_t head_entry_cap,
  size_t entry_cap_in_region
) {
  size_t _head_entry_cap = (head_entry_cap <= 0)
                           ? DEFAULT_HEAD_ENTRY_CAP
                           : head_entry_cap;
  size_t _entry_cap_in_region = (entry_cap_in_region <= 0)
                                ? DEFAULT_ENTRY_CAP_IN_REGION
                                : entry_cap_in_region;

  size_t entry_size = sizeof(HashDictEntry);
  assert(
    (_entry_cap_in_region * entry_size) >=
    (_head_entry_cap * entry_size)
  );

  log_trace("beg alloc hashdict [arena=%p, head_cap=%lld]", arena,
            _head_entry_cap);

  // TODO
  // assert(
  //   _head_entry_cap % 2 == 0 &&
  //   "head_entry_cap must be multiple of 2 for optimized hashing"
  // );

  HashDict *hd;
  if (arena) {
    hd = arena_malloc(arena, sizeof(HashDict));
  } else {
    hd = (HashDict *)malloc(sizeof(HashDict));
  }

  if (!hd) {
    log_fatal("alloc hashdict [arena=%p]", arena);
    return NULL;
  }

  hd->is_in_arena = (arena != NULL);
  hd->head_entry_cap = _head_entry_cap;
  hd->entries_arena = arena_alloc(
                        _entry_cap_in_region * sizeof(HashDictEntry)
                      );
  hd->entry_count = 0;

  // Arena's calloc already sets nexts to NULL, hopefully.
  hd->heads = arena_malloc(
                hd->entries_arena,
                _head_entry_cap * entry_size
              );

  log_debug("end alloc hashdict(%p) [arena=%p, head_cap=%lld, heads=%p]", hd,
            arena,
            _head_entry_cap, hd->heads);

  return hd;
}

void hashdict_free(HashDict *hd) {
  arena_free(hd->entries_arena);

  log_debug("free hashdict(%p) [in_arena?=%b]", hd, hd->is_in_arena);
  if (!hd->is_in_arena) {
    free(hd);
  }
}

HashDictEntry *hashdict_add_entry(
  HashDict *hd,
  char *key,
  size_t key_size,
  char *val,
  size_t val_size
) {
  assert(hd);
  assert(key);
  assert(key_size >= 1);
  assert(val);
  assert(val_size >= 1);
  assert(hd->head_entry_cap > 0);

  uint64_t key_hash = murmurhash3(key, (int)key_size);
  uint64_t hashed_idx = key_hash % hd->head_entry_cap;

  // Key and value aren't necessarily chars, but it helps when looking at logs.
  log_trace("beg hd_add_entry [hd=%p, idx=%lld, k=%p, v=%p]", hd, hashed_idx,
            key, val);

  HashDictEntry *head = (hd->heads + hashed_idx);
  // Since Heads are already allocated, key and val are our signals
  // that it is empty.
  if (!head->key && !head->val) {
    head->key = key;
    head->val = val;
    head->key_size = key_size;
    head->val_size = val_size;

    ++hd->entry_count;

    log_debug("end hd_add_entry entry(%p) [hd=%p, k=%p, v=%p, count=%lld]",
              head, hd, key, val, hd->entry_count);

    return head;
  }

  HashDictEntry *last_entry = head;
  HashDictEntry *cur_entry = head->next;
  while (cur_entry) {
    last_entry = cur_entry;
    cur_entry = cur_entry->next;
  }

  HashDictEntry *new_entry = arena_malloc(
                               hd->entries_arena,
                               sizeof(HashDictEntry)
                             );
  new_entry->key = key;
  new_entry->val = val;
  new_entry->key_size = key_size;
  new_entry->val_size = val_size;
  last_entry->next = new_entry;

  ++hd->entry_count;

  log_debug("end hd_add_entry entry(%p) [hd=%p, k=%p, v=%p, count=%lld]",
            new_entry,
            hd, key, val, hd->entry_count);

  return new_entry;
}

HashDictEntry *hashdict_get_entry(
  HashDict *hd,
  char *key,
  size_t key_size
) {
  assert(hd);
  assert(key);
  assert(key_size >= 1);
  assert(hd->head_entry_cap > 0);

  uint64_t key_hash = murmurhash3(key, (int)key_size);
  uint64_t hashed_idx = key_hash % hd->head_entry_cap;

  log_trace("beg hd_get_entry [hd=%p, idx=%lld]", hd, hashed_idx);

  HashDictEntry *cur_entry = (hd->heads + hashed_idx);
  while (cur_entry) {
    if (!cur_entry->key) {
      continue;
    } else if (strncmp(cur_entry->key, key, key_size) == 0) {
      break;
    }

    cur_entry = cur_entry->next;
  }

  log_debug("end hd_get_entry entry(%p) [hd=%p, k=%p, v=%p]", cur_entry, hd,
            cur_entry->key, cur_entry->val);

  return cur_entry;
}
