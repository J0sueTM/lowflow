#include "./stack.h"

void lf_init_stack(LF_Stack *stack) {
  assert(stack);
  assert(stack->elem_size > 0);
  assert(stack->elem_qtt_in_block > 0);

  size_t padding = (
    (stack->elem_alignment - (stack->elem_size % stack->elem_alignment))
    % stack->elem_alignment
  );
  stack->elem_padded_size = stack->elem_size + padding;
  size_t block_size = stack->elem_qtt_in_block * stack->elem_padded_size;
  LF_Arena *arena = &stack->arena;
  arena->head_block.data = (char *)malloc(block_size);
  assert(arena->head_block.data);

  arena->block_count = 1;
  arena->head_block.offset = 0;
  arena->head_block.size = block_size;
  arena->head_block.next = NULL;
  arena->top_block = &arena->head_block;
  arena->cursor_block = &arena->head_block;
}

char *lf_alloc_stack_elem(LF_Stack *stack) {
  assert(stack);

  LF_Arena *arena = &stack->arena;
  LF_MemBlock *top_block = arena->top_block;

  size_t filled_offset = top_block->offset + stack->elem_size;
  if (filled_offset <= arena->block_size) {
    char *elem = top_block->data + filled_offset;
    top_block->offset = filled_offset;
    return elem;
  }

  LF_MemBlock *new_block = (LF_MemBlock *)malloc(sizeof(LF_MemBlock));
  assert(new_block);
  new_block->offset = filled_offset;
  new_block->size = arena->block_size;
  new_block->data = (char *)malloc(arena->block_size);
  new_block->next = NULL;
  ++arena->block_count;

  top_block->next = new_block;
  arena->top_block = new_block;

  return new_block->data;
}

void lf_reset_stack(LF_Stack *stack) {
  assert(stack);

  LF_Arena *arena = &stack->arena;
  LF_MemBlock *cur_block = &arena->head_block;
  while (cur_block) {
    cur_block->offset = 0;
    cur_block = cur_block->next;
  }
}

char *lf_pop_from_stack(LF_Stack *stack) {
  assert(stack);

  LF_Arena *arena = &stack->arena;
  char *elem = arena->top_block->data - stack->elem_padded_size;
  arena->top_block->offset -= stack->elem_padded_size;
  return elem;
}
