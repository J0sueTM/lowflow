#ifndef LF_ARENA_H
#define LF_ARENA_H

#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct MemRegion MemRegion;
typedef struct MemRegion {
  void *data;
  size_t cap;
  size_t used;
  size_t remainder; // Helper.
  MemRegion *next;
} MemRegion;

#define DEFAULT_ARENA_CAP 1024
typedef struct Arena {
  // Stack based linked list.
  MemRegion *top;
  size_t region_cap;
} Arena;

Arena *arena_alloc(size_t region_cap);
void arena_reset(Arena *arena);
void arena_free(Arena *arena);

void *arena_malloc(Arena *arena, size_t size);

#endif // LF_ARENA_H
