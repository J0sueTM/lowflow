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

#include "./arena.h"

static void region_free(MemRegion *top, bool should_recurse) {
  assert(top);
  assert(top->data);

  MemRegion *next_region = top->next;
  free(top->data);
  free(top);
  
  if (!next_region || !should_recurse) {
    return;
  }

  region_free(next_region, true);

  top->next = NULL;
}


// TODO: String manipulation.

Arena *arena_alloc(size_t region_cap) {
  Arena *arena = (Arena *)malloc(sizeof(Arena));
  assert(arena && "failed to alloc arena");

  arena->region_cap = (region_cap <= 1)
    ? DEFAULT_ARENA_CAP
    : region_cap;

  arena->top = (MemRegion *)malloc(sizeof(MemRegion));
  assert(arena->top && "failed to alloc top region");

  arena->top->data = (char *)calloc(arena->region_cap, sizeof(char));
  assert(arena->top->data && "failed to calloc top region's data");

  arena->top->cap = arena->region_cap;
  arena->top->remainder = arena->region_cap;

  // Pedantic but good documentation.
#ifndef NDEBUG
  arena->top->used = 0;
  arena->top->next = NULL;
#endif

  return arena;
}

void arena_reset(Arena *arena) {
  assert(arena);
  assert(arena->top);

  if (arena->top->next) {
    region_free(arena->top->next, true);
  }
  memset(arena->top->data, 0x00, arena->top->used);
  arena->top->used = 0;
  arena->top->remainder = arena->region_cap;
  arena->top->next = NULL;
}

void arena_free(Arena *arena) {
  assert(arena);
  assert(arena->top);

  region_free(arena->top, true);
  free(arena);
  arena = NULL;
}

void *arena_malloc(Arena *arena, size_t size) {
  assert(arena);

  // TODO: Break data bigger than region_cap into multiple regions.
  //
  // Optimally that wouldn't be necessary because in most cases we
  // should know an approximately good size for our regions. But
  // there are cases where that wouldn't be the case, primarily when
  // we are not the ones allocating stuff, i.e. the end user of this
  // language.
  assert(
    size <= arena->region_cap &&
    "tried to malloc block bigger than region_cap"
  );

  MemRegion *region = arena->top;
  if (size <= region->remainder) {
    void *ptr = (region->data + region->used);
    region->used += size;
    region->remainder -= size;

    return ptr;
  }

  MemRegion *new_region = (MemRegion *)malloc(sizeof(MemRegion));
  assert(new_region && "failed to alloc new region");

  new_region->cap = arena->region_cap;
  new_region->used = size;
  new_region->remainder = arena->region_cap - size;
  new_region->next = arena->top;
  arena->top = new_region;

  new_region->data = (char *)calloc(arena->region_cap, sizeof(char));
  assert(new_region->data && "failed to alloc new region's data");

  return new_region->data;
}
