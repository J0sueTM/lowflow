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

#ifndef LF_HASHDICT_H
#define LF_HASHDICT_H

#include <stddef.h>
#include <stdlib.h>

#include "./arena.h"
#include "./superhash.h"

typedef struct HashDictEntry HashDictEntry;
typedef struct HashDictEntry {
  char *key;
  void *val;
  size_t key_size;
  size_t val_size;

  HashDictEntry *next;
} HashDictEntry;

#define DEFAULT_HEAD_ENTRY_CAP 128
#define DEFAULT_ENTRY_CAP_IN_REGION 50
typedef struct HashDict {
  HashDictEntry *heads;
  size_t head_entry_cap;

  Arena *entries_arena;
} HashDict;


HashDict *hashdict_alloc(
  size_t head_entry_cap,
  size_t entry_cap_in_region
);
void hashdict_dealloc(HashDict *hd);

HashDictEntry *hashdict_add_entry(
  HashDict *hd,
  const char *key,
  void *val,
  size_t key_size,
  size_t val_size
);

#endif
