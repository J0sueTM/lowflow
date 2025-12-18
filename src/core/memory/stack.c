#include "./stack.h"

void lf_init_stack(LF_Stack *stack) {
  assert(stack);
  assert(stack->elem_size > 0);
  assert(stack->elem_qtt_in_block > 0);

  stack->elem_count = 0;
  size_t padding =
    ((stack->elem_alignment - (stack->elem_size % stack->elem_alignment)) %
     stack->elem_alignment);
  stack->elem_padded_size = stack->elem_size + padding;

  stack->arena.block_size =
    (stack->elem_qtt_in_block * stack->elem_padded_size);
  stack->arena.block_offset = LF_MEMBLOCK_OFFSET_LEFT;
  lf_init_arena(&stack->arena);
}

char *lf_alloc_stack_elem(LF_Stack *stack) {
  assert(stack);

  char *elem = lf_arena_alloc_at_tail(&stack->arena, stack->elem_size);
  ++stack->elem_count;

  return elem;
}

void lf_reset_stack(LF_Stack *stack) {
  assert(stack);

  lf_reset_arena(&stack->arena);
}

char *lf_pop_from_stack(LF_Stack *stack) {
  assert(stack);

  if (stack->elem_count <= 0) {
    return NULL;
  }

  LF_Arena *arena = &stack->arena;
  LF_MemBlock *tail_block = arena->tail_block;
  bool is_block_empty = tail_block->right_offset == 0;
  if (is_block_empty) {
    tail_block = lf_dealloc_arena_tail_block(arena);
  }

  size_t new_right_offset = tail_block->right_offset - stack->elem_padded_size;
  if (new_right_offset <= tail_block->left_offset) {
    tail_block->left_offset = new_right_offset;
  }
  tail_block->right_offset = new_right_offset;

  char *elem = tail_block->data + new_right_offset;
  --stack->elem_count;

  return elem;
}

bool lf_is_stack_empty(LF_Stack *stack) {
  assert(stack);

  return stack->elem_count == 0;
}

char *lf_get_stack_elem_by_content(LF_Stack *stack, char *content) {
  assert(stack);

  return lf_get_arena_elem_by_content(&stack->arena,
                                      content,
                                      stack->elem_size,
                                      stack->elem_padded_size);
}
