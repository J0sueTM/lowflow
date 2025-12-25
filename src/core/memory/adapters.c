#include "./adapters.h"

void lf_stack_to_list(LF_Stack *stack, LF_List *list) {
  assert(stack);
  assert(list);
  assert(list->elem_size == stack->elem_size);
  assert(list->elem_alignment == stack->elem_alignment);
  assert(list->elem_padded_size == stack->elem_padded_size);
  assert(list->elem_qtt_in_block == stack->elem_qtt_in_block);
  assert(list->arena.block_size == stack->arena.block_size);

  // TODO: Most of this can be extracted as arena logic.

  size_t missing_block_qtt = stack->arena.block_count - list->arena.block_count;
  LF_MemBlock *remaining_blocks = NULL;
  char *remaining_data = NULL;
  if (missing_block_qtt >= 1) {
    remaining_blocks =
      (LF_MemBlock *)malloc(sizeof(LF_MemBlock) * missing_block_qtt);
    assert(remaining_blocks);

    remaining_data =
      (char *)malloc(sizeof(char) * list->arena.block_size * missing_block_qtt);
    assert(remaining_data);
  }

  size_t copied_block_qtt = 0;
  LF_MemBlock *cur_stack_block = stack->arena.head_block;
  LF_MemBlock *cur_list_block = list->arena.head_block;
  LF_MemBlock *last_list_block = NULL;
  while (copied_block_qtt < list->arena.block_count) {
    size_t stack_block_used_size =
      cur_stack_block->right_offset - cur_stack_block->left_offset;
    memcpy(cur_list_block->data + cur_stack_block->left_offset,
           cur_stack_block->data + cur_stack_block->left_offset,
           stack_block_used_size);
    assert(cur_list_block->data + cur_stack_block->left_offset);

    cur_list_block->left_offset = cur_stack_block->left_offset;
    cur_list_block->right_offset = cur_stack_block->right_offset;

    if (last_list_block) {
      last_list_block->next = cur_list_block;
      cur_list_block->prev = last_list_block;
    }

    cur_stack_block = cur_stack_block->next;
    last_list_block = cur_list_block;
    cur_list_block = cur_list_block->next;
    ++copied_block_qtt;
  }

  cur_list_block = remaining_blocks;
  char *cur_list_block_data = remaining_data;
  while (copied_block_qtt < stack->arena.block_count) {
    size_t stack_block_used_size =
      cur_stack_block->right_offset - cur_stack_block->left_offset;
    memcpy(cur_list_block_data + cur_stack_block->left_offset,
           cur_stack_block->data + cur_stack_block->left_offset,
           stack_block_used_size);
    cur_list_block->data = cur_list_block_data;
    assert(cur_list_block_data + cur_stack_block->left_offset);

    cur_list_block->left_offset = cur_stack_block->left_offset;
    cur_list_block->right_offset = cur_stack_block->right_offset;

    last_list_block->next = cur_list_block;
    cur_list_block->prev = last_list_block;

    last_list_block = cur_list_block;
    cur_stack_block = cur_stack_block->next;

    ++cur_list_block;
    cur_list_block_data += list->arena.block_size;

    ++copied_block_qtt;
  }

  list->elem_count = stack->elem_count;
  list->arena.block_count = stack->arena.block_count;
  list->arena.tail_block = last_list_block;
}
