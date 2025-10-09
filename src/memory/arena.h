#ifndef LF_ARENA_H
#define LF_ARENA_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>

typedef struct LF_MemBlock LF_MemBlock;
typedef struct LF_MemBlock {
  char *data;
  size_t offset;
  size_t size;

  // TODO: Implement me.
  // size_t idx;
  LF_MemBlock *prev;
  LF_MemBlock *next;
} LF_MemBlock;

typedef struct LF_Arena {
  LF_MemBlock head_block;
  LF_MemBlock *top_block;
  size_t block_count;
  size_t block_size;

  LF_MemBlock *cursor_block;
} LF_Arena;

void lf_init_arena(LF_Arena *arena);

char *lf_arena_alloc(LF_Arena *arena, size_t size);

void lf_reset_arena(LF_Arena *arena);

#endif // LF_ARENA_H
