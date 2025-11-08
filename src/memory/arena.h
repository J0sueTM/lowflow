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

  LF_MemBlock *prev;
  LF_MemBlock *next;
} LF_MemBlock;

typedef struct LF_Arena {
  LF_MemBlock *head_block;
  LF_MemBlock *tail_block;
  size_t block_count;
  size_t block_size;

  LF_MemBlock *cursor_block;
} LF_Arena;

void lf_init_arena(LF_Arena *arena);

LF_MemBlock *lf_alloc_arena_memblock(LF_Arena *arena);

LF_MemBlock *lf_dealloc_arena_head_memblock(LF_Arena *arena);

LF_MemBlock *lf_dealloc_arena_tail_memblock(LF_Arena *arena);

char *lf_arena_alloc(LF_Arena *arena, size_t size);

void lf_reset_arena(LF_Arena *arena);

// Assumes same sized elements.
char *lf_get_arena_elem_by_content(
  LF_Arena *arena,
  char *content,
  size_t elem_size,
  size_t elem_padded_size
);

#endif // LF_ARENA_H
