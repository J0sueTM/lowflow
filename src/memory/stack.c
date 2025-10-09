#include "./stack.h"

void lf_init_stack(LF_Stack *stack) {
  assert(stack);
  assert(stack->elem_size > 0);
  assert(stack->elem_qtt_in_block > 0);

  stack->elem_count = 0;
  size_t padding = (
    (stack->elem_alignment - (stack->elem_size % stack->elem_alignment))
    % stack->elem_alignment
  );
  stack->elem_padded_size = stack->elem_size + padding;

  stack->arena.block_size = (
    stack->elem_qtt_in_block * stack->elem_padded_size
  );
  lf_init_arena(&stack->arena);
}

char *lf_alloc_stack_elem(LF_Stack *stack) {
  assert(stack);

  char *elem = lf_arena_alloc(&stack->arena, stack->elem_size);
  ++stack->elem_count;

  return elem;
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
  assert(stack->elem_count > 0);

  LF_Arena *arena = &stack->arena;
  char *elem = (
    arena->top_block->data +
    ((stack->elem_count - 1) * stack->elem_padded_size)
  );
  arena->top_block->offset -= stack->elem_padded_size;
  return elem;
}
