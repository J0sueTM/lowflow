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
  stack->arena.block_offset = LF_MEMBLOCK_OFFSET_RIGHT;
  lf_init_arena(&stack->arena);
}

char *lf_alloc_stack_elem(LF_Stack *stack) {
  assert(stack);

  char *elem = lf_arena_alloc_at_head(&stack->arena, stack->elem_size);
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
  LF_MemBlock *head_block = arena->head_block;
  if (lf_is_block_empty(head_block)) {
    head_block = lf_dealloc_arena_head_block(arena);
  }

  size_t new_left_offset = head_block->left_offset + stack->elem_padded_size;
  // This prevents phantom data from showing up if somehow
  // offsets don't align and when popping, the left offset
  // goes beyond the right offset.
  if (new_left_offset > head_block->right_offset) {
    head_block->right_offset = new_left_offset;
  }

  char *elem = head_block->data + head_block->left_offset;

  head_block->left_offset = new_left_offset;
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

void lf_debug_stack(LF_Stack *stack, void(*debug_fn)(char *data)) {
  assert(stack);

  LF_MemBlock *cur_block = stack->arena.head_block;
  while (cur_block) {
    size_t used_size = cur_block->right_offset - cur_block->left_offset;
    bool is_empty = used_size <= 0;
    if (is_empty) {
      cur_block = cur_block->next;
      continue;
    }

    for (
      size_t i = cur_block->left_offset;
      i <= cur_block->right_offset - stack->elem_padded_size;
      i += stack->elem_padded_size
    ) {
      debug_fn(cur_block->data + i);
    }
    printf("\n");

    cur_block = cur_block->next;
  }
}
