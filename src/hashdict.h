/*
 * Copyright (C) Josué Teodoro Moreira
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <https://www.gnu.org/licenses/>.
 */

#ifndef LF_HASHDICT_H
#define LF_HASHDICT_H

#include <stddef.h>
#include <stdlib.h>

#include "../libs/log.c/src/log.h"
#include "./arena.h"
#include "./murmurhash3.h"

typedef struct HashDictEntry HashDictEntry;
typedef struct HashDictEntry
{
    char *key;
    char *val;
    size_t key_size;
    size_t val_size;

    HashDictEntry *next;
} HashDictEntry;

#define DEFAULT_HEAD_ENTRY_CAP 64
#define DEFAULT_ENTRY_CAP_IN_REGION 128
typedef struct HashDict
{
    bool are_entries_in_outter_arena;
    bool is_in_arena;
    size_t entry_count;

    HashDictEntry *heads;
    size_t head_entry_cap;

    Arena *entries_arena;
} HashDict;

HashDict *
hashdict_alloc(Arena *hds_arena,
               Arena *entries_arena,
               size_t head_entry_cap,
               size_t entry_cap_in_region);

void
hashdict_free(HashDict *hd);

HashDictEntry *
hashdict_add_entry(HashDict *hd,
                   char *key,
                   size_t key_size,
                   char *val,
                   size_t val_size);

HashDictEntry *
hashdict_get_entry(HashDict *hd, char *key, size_t key_size);

#endif
