#ifndef LF_ARENA_H
#define LF_ARENA_H

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct LF_MemBlock LF_MemBlock;
typedef struct LF_MemBlock {
  char *data;
  size_t right_offset;
  size_t left_offset;
  size_t size;

  LF_MemBlock *prev;
  LF_MemBlock *next;
} LF_MemBlock;

// Since head blocks begin with their offset all the way
// to the end of the block, and tail blocks begin with
// their offset in the beginning of the block, either
// having the offset of the first block in these extremes
// make so an opposite operation will allocate memory
// again. This probably makes both happy.
//
// IMPORTANT: This makes so all arenas should be initialized
// with an even number of elements bigger or equal to 2.
typedef enum LF_MemBlockOffset {
  LF_MEMBLOCK_OFFSET_MIDDLE,
  LF_MEMBLOCK_OFFSET_LEFT,
  LF_MEMBLOCK_OFFSET_RIGHT
} LF_MemBlockOffset;

typedef struct LF_Arena {
  LF_MemBlock *head_block;
  LF_MemBlock *tail_block;
  size_t block_count;
  size_t block_size;

  LF_MemBlockOffset block_offset;

  LF_MemBlock *cursor_block;
} LF_Arena;

void lf_init_arena(LF_Arena *arena);

LF_MemBlock *lf_alloc_arena_head_block(LF_Arena *arena);
LF_MemBlock *lf_alloc_arena_tail_block(LF_Arena *arena);

void lf_dealloc_arena_block(LF_Arena *arena, LF_MemBlock *block);
LF_MemBlock *lf_dealloc_arena_head_block(LF_Arena *arena);
LF_MemBlock *lf_dealloc_arena_tail_block(LF_Arena *arena);

char *lf_arena_alloc_at_head(LF_Arena *arena, size_t size);
char *lf_arena_alloc_at_tail(LF_Arena *arena, size_t size);

void lf_reset_arena(LF_Arena *arena);

void lf_dealloc_arena(LF_Arena *arena);

bool lf_is_block_empty(LF_MemBlock *block);

// Assumes same sized elements.
char *lf_get_arena_elem_by_content(LF_Arena *arena,
                                   char *content,
                                   size_t elem_size,
                                   size_t elem_padded_size);

#endif // LF_ARENA_H
