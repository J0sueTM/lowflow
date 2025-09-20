#include <assert.h>
#include <stdio.h>

#include "./arena.h"

void lf_init_arena(LF_Arena *arena) {
  assert(arena);
  assert(arena->block_size >= 0);

  arena->block_count = 1;
  arena->head_block.data = (char *)calloc(1, arena->block_size);
  assert(arena->head_block.data);
  arena->head_block.offset = 0;
  arena->head_block.size = arena->block_size;
  arena->top_block = &arena->head_block;
}

char *lf_arena_alloc(LF_Arena *arena, size_t size, size_t alignment) {
  assert(arena);

  LF_MemBlock *top_block = arena->top_block;

  size_t misalignment = (alignment != 0) ? top_block->offset % alignment : 0;
  size_t aligned_offset = top_block->offset + ((misalignment == 0) ? 0 : (alignment - misalignment));
  size_t filled_offset = aligned_offset + size;
  if (filled_offset <= arena->block_size) {
    char *ptr = top_block->data + aligned_offset;
    top_block->offset = filled_offset;
    return ptr;
  }

  LF_MemBlock *new_block = (LF_MemBlock *)malloc(sizeof(LF_MemBlock));
  assert(new_block);
  new_block->offset = size;
  new_block->size = arena->block_size;
  new_block->data = (char *)calloc(1, arena->block_size);
  ++arena->block_count;

  top_block->next = new_block;
  arena->top_block = new_block;

  return new_block->data;
}

// WARNING: This function assumes that every block contains the same
// element of the same size and alignment.
char *lf_arena_get_elem_by_pos(
  LF_Arena *arena,
  size_t pos,
  size_t size,
  size_t alignment
) {
  assert(arena);

  size_t padding = (alignment - (size % alignment)) % alignment;
  size_t filled_size = size + padding;
  size_t elems_in_block = arena->block_size / filled_size;
  size_t block_pos = pos / elems_in_block;
  size_t elem_pos_in_block = pos % elems_in_block;

  // TODO: Optimization. Start from the top or the head depending
  // on distance.
  LF_MemBlock *cur_block = &arena->head_block;
  for (size_t i = 0; i < block_pos; ++i) {
    cur_block = cur_block->next;
  }
  assert(cur_block);

  size_t offset = elem_pos_in_block * filled_size;
  return (cur_block->data + offset);
}
