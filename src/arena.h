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

#ifndef LF_ARENA_H
#define LF_ARENA_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../libs/log.c/src/log.h"

typedef struct MemRegion MemRegion;
typedef struct MemRegion
{
    char *data;
    size_t cap;
    size_t used;
    size_t remainder; // Helper.
    MemRegion *next;
} MemRegion;

#define DEFAULT_ARENA_CAP 1024
typedef struct Arena
{
    // Stack based linked list.
    MemRegion *top;
    size_t region_cap;
} Arena;

Arena *
arena_alloc(size_t region_cap);

void
arena_reset(Arena *arena);

void
arena_free(Arena *arena);

void *
arena_malloc(Arena *arena, size_t size);

#endif // LF_ARENA_H
