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

Arena *arena_alloc(size_t region_cap) {
  Arena *arena = (Arena *)malloc(sizeof(Arena));
  assert(arena && "failed to alloc arena");

  arena->region_cap = (region_cap <= 1)
    ? DEFAULT_ARENA_CAP
    : region_cap;

  arena->top = (MemRegion *)malloc(sizeof(MemRegion));
  assert(arena->top && "failed to alloc top region");

  arena->top->data = (void *)calloc(sizeof(char), arena->region_cap);
  assert(arena->top->data && "failed to calloc top region's data");

  arena->top->next = NULL;
  arena->top->cap = arena->region_cap;
  arena->top->remainder = arena->region_cap;
  arena->top->used = 0;

  return arena;
}

void arena_reset(Arena *arena) {
  assert(arena);
  assert(arena->top);

  region_free(arena->top->next, true);
  memset(arena->top->data, 0b0, arena->top->used);
  arena->top->used = 0;
  arena->top->remainder = arena->region_cap;
  arena->top->next = NULL;
}

void arena_free(Arena *arena) {
  assert(arena);
  assert(arena->top);

  region_free(arena->top, true);
  free(arena);
}

void *arena_malloc(Arena *arena, size_t size) {
  assert(arena);

  if (size > arena->region_cap) {
    fprintf(
      stderr,
      "tried to alloc %ld, but arena's region cap is %ld\n",
      size,
      arena->region_cap
      );
      return NULL;
  }

  MemRegion *region = arena->top;
  if (region->remainder >= size) {
    void *ptr = (region->data + region->used + 1);
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

  new_region->data = (void *)malloc(arena->region_cap);
  assert(new_region->data && "failed to alloc new region's data");

  return new_region->data;
}
