#include <assert.h>
#include <stdio.h>

#include "./arena.h"

static size_t _align_forward(size_t offset, size_t alignment) {
    size_t misalignment = offset % alignment;
    if (misalignment == 0) return offset;
    return offset + (alignment - misalignment);
}

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
  size_t aligned_offset = _align_forward(top_block->offset, alignment);

  size_t new_offset_aligned = top_block->offset + aligned_offset;
  size_t new_offset_filled = new_offset_aligned + size;
  printf("%ld %ld %ld\n", new_offset_aligned, new_offset_filled, arena->block_size);
  if (new_offset_filled <= arena->block_size) {
    char *ptr = top_block->data + new_offset_aligned;
    top_block->offset = new_offset_filled;
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
