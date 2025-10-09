#include "./arena.h"

void lf_init_arena(LF_Arena *arena) {
  assert(arena);
  assert(arena->block_size);

  arena->head_block.data = (char *)malloc(arena->block_size);
  assert(arena->head_block.data);

  arena->block_count = 1;
  arena->block_size = arena->block_size;
  arena->head_block.offset = 0;
  arena->head_block.size = arena->block_size;
  arena->head_block.next = NULL;
  arena->top_block = &arena->head_block;
  arena->cursor_block = &arena->head_block;
}

char *lf_arena_alloc(LF_Arena *arena, size_t size) {
  assert(arena);
  assert(size > 0);

  LF_MemBlock *top_block = arena->top_block;
  if (size <= arena->block_size) {
    size_t filled_offset = top_block->offset + size;
    char *ptr = top_block->data + top_block->offset;
    top_block->offset = filled_offset;
    return ptr;
  }

  LF_MemBlock *new_block = (LF_MemBlock *)malloc(sizeof(LF_MemBlock));
  assert(new_block);
  new_block->offset = size;
  new_block->size = arena->block_size;
  new_block->next = NULL;
  ++arena->block_count;
  
  new_block->data = (char *)malloc(arena->block_size);
  assert(new_block->data);

  top_block->next = new_block;
  arena->top_block = new_block;

  return new_block->data;
}

void lf_reset_arena(LF_Arena *arena) {
  assert(arena);

  LF_MemBlock *cur_block = &arena->head_block;
  while (cur_block) {
    cur_block->offset = 0;
    cur_block = cur_block->next;
  }
}
