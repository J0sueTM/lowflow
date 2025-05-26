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

#include "./arena.h"

static size_t
region_free(MemRegion *top, bool should_recurse)
{
    assert(top);
    assert(top->data);

    size_t bytes_freed = top->cap;
    MemRegion *next_region = top->next;

    log_debug("free data(%p) [region=%p, used=%lld]",
              top->data,
              bytes_freed,
              top->data,
              top->used);
    free(top->data);

    log_debug("free mregion(%p) [cap=%lld]", top, bytes_freed);
    free(top);

    if (!next_region || !should_recurse) {
        return bytes_freed;
    }

    return bytes_freed + region_free(next_region, should_recurse);
}

// TODO: String manipulation.
Arena *
arena_alloc(size_t region_cap)
{
    log_trace("beg alloc arena [reg_cap=%lld]", region_cap);

    Arena *arena = (Arena *)malloc(sizeof(Arena));
    if (!arena) {
        log_fatal("alloc arena [region_cap=%lld]", region_cap);
        return NULL;
    }
    arena->region_cap = (region_cap <= 1) ? DEFAULT_ARENA_CAP : region_cap;

    arena->top = (MemRegion *)malloc(sizeof(MemRegion));
    if (!arena->top) {
        log_fatal("alloc mregion [arena=%p]", arena);
        free(arena);
        return NULL;
    }
    log_debug("alloc mregion(%p) [arena=%p]", arena->top, arena);

    arena->top->data = (char *)calloc(arena->region_cap, sizeof(char));
    if (!arena->top->data) {
        log_fatal("alloc data [cap=%lld, arena=%p, mregion=%p]",
                  arena->region_cap,
                  arena,
                  arena->top);
        free(arena->top);
        free(arena);
        return NULL;
    }
    log_debug("alloc data(%p) [cap=%lld, arena=%p, mregion=%p]",
              arena->top->data,
              arena->region_cap,
              arena,
              arena->top);

    arena->top->cap = arena->region_cap;
    arena->top->remainder = arena->region_cap;
    arena->top->used = 0;
    arena->top->next = NULL;

    log_debug(
      "end alloc arena(%p) [region_cap=%lld]", *arena, arena->region_cap);

    return arena;
}

void
arena_reset(Arena *arena)
{
    assert(arena);
    assert(arena->top);

    if (arena->top->next) {
        region_free(arena->top->next, true);
    }
    memset(arena->top->data, 0x00, arena->top->used);
    arena->top->used = 0;
    arena->top->remainder = arena->region_cap;
    arena->top->next = NULL;

    log_debug("reset arena(%p)", arena);
}

void
arena_free(Arena *arena)
{
    assert(arena);
    assert(arena->top);

    size_t bytes_freed = region_free(arena->top, true);
    log_debug("free arena(%p) [bytes=%lld]", arena, bytes_freed);

    free(arena);
}

void *
arena_malloc(Arena *arena, size_t size)
{
    assert(arena);

    // TODO: Break data bigger than region_cap into multiple regions.
    //
    // Optimally that wouldn't be necessary because in most cases we
    // should know an approximately good size for our regions. But there
    // are cases where that wouldn't be the case, primarily when we are
    // not the ones allocating stuff, i.e. the end user of this
    // language.
    assert(size <= arena->region_cap &&
           "tried to malloc block bigger than region_cap");

    log_trace("beg alloc mblock [arena=%p, size=%lld]", arena, size);

    MemRegion *region = arena->top;
    if (size <= region->remainder) {
        void *ptr = (region->data + region->used);
        region->used += size;
        region->remainder -= size;
        log_debug("end alloc mblock(%p) [size=%lld, arena=%p, mregion=%p]",
                  ptr,
                  size,
                  arena,
                  region);

        return ptr;
    }

    log_trace("alloc mregion [arena=%p]", arena);
    MemRegion *new_region = (MemRegion *)malloc(sizeof(MemRegion));
    if (!new_region) {
        log_fatal("alloc mregion [arena=%p]", arena);
        return NULL;
    }
    new_region->cap = arena->region_cap;
    new_region->used = size;
    new_region->remainder = arena->region_cap - size;

    new_region->data = (char *)calloc(arena->region_cap, sizeof(char));
    if (!new_region->data) {
        log_fatal("alloc data [arena=%p, mregion=%p]", arena, new_region);
        free(new_region);
        return NULL;
    }
    log_debug("alloc data(%p) [arena=%p, region=%p, size=%lld]",
              new_region->data,
              arena,
              new_region,
              size);

    new_region->next = arena->top;
    arena->top = new_region;
    log_debug("alloc mregion(%p) [arena=%p, cap=%ll, used=%d]",
              new_region,
              arena,
              arena->region_cap,
              size);

    // Just to conform to the default behaviour (not creating new
    // mregion).
    char *ptr = new_region->data;
    log_debug("end alloc mblock(%p) [arena=%p, size=%lld, mregion=%p]",
              ptr,
              size,
              arena,
              region);

    return ptr;
}
