#ifndef LF_ARENA_H
#define LF_ARENA_H

#include <stdlib.h>

typedef struct LF_MemBlock LF_MemBlock;
typedef struct LF_MemBlock {
  char *data;
  size_t offset;
  size_t size;

  LF_MemBlock *next;
} LF_MemBlock;

typedef struct LF_Arena {
  LF_MemBlock head_block;
  LF_MemBlock *top_block;
  size_t block_count;
  size_t block_size;
} LF_Arena;

void lf_init_arena(LF_Arena *arena);
char *lf_arena_alloc(LF_Arena *arena, size_t size, size_t alignment);

#endif // LF_ARENA_H
