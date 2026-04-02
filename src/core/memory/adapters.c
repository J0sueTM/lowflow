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

void _lf_get_arena_slice_size(LF_ArenaSlice *slice) {
  assert(slice);

  if (slice->head_block == slice->tail_block) {
    slice->size = slice->tail_offset - slice->head_offset;
    return;
  }

  size_t head_block_owned_size = slice->head_block->size - slice->head_offset;
  size_t tail_block_owned_size = slice->tail_offset;
  size_t size = head_block_owned_size + tail_block_owned_size;
  LF_MemBlock *cur_block = slice->head_block->next;
  while (cur_block && cur_block != slice->tail_block) {
    size_t used_size = cur_block->right_offset - cur_block->left_offset;
    size += used_size;
    cur_block = cur_block->next;
  }

  slice->size = size;
}

void lf_init_arena_slice(LF_ArenaSlice *dest, LF_Arena *src) {
  assert(dest);

  if (src) {
    dest->head_block = src->head_block;
    dest->tail_block = src->tail_block;
    dest->head_offset = src->head_block->left_offset;
    dest->tail_offset = src->tail_block->right_offset;
  }
  assert(dest->head_block);
  assert(dest->tail_block);

  _lf_get_arena_slice_size(dest);
}

char *lf_arena_slice_alloc_at_head(LF_ArenaSlice *slice,
                                   LF_Arena *arena,
                                   size_t size) {
  assert(slice);
  assert(arena);

  bool lost_head = slice->head_block != arena->head_block;
  if (lost_head) {
    return NULL;
  }

  bool lost_offset = slice->head_offset > arena->head_block->left_offset;
  if (lost_offset) {
    return NULL;
  }

  char *ptr = lf_arena_alloc_at_head(arena, size);
  assert(ptr);
  slice->head_block = arena->head_block;
  slice->head_offset = arena->head_block->left_offset;

  slice->size += size;

  return ptr;
}

char *lf_arena_slice_alloc_at_tail(LF_ArenaSlice *slice,
                                   LF_Arena *arena,
                                   size_t size) {
  assert(slice);
  assert(arena);

  bool lost_tail = slice->tail_block != arena->tail_block;
  if (lost_tail) {
    return NULL;
  }

  bool lost_offset = slice->tail_offset < arena->tail_block->right_offset;
  if (lost_offset) {
    return NULL;
  }

  char *ptr = lf_arena_alloc_at_tail(arena, size);
  assert(ptr);
  slice->tail_block = arena->tail_block;
  slice->tail_offset = arena->tail_block->right_offset;

  slice->size += size;

  return ptr;
}

bool lf_grow_arena_slice_tail(LF_ArenaSlice *slice,
                              LF_Arena *arena,
                              size_t size) {
  assert(slice);
  assert(arena);

  size_t tail_remainder = slice->tail_block->size - slice->tail_offset;
  if (size <= tail_remainder) {
    slice->tail_offset += size;
    slice->size += size;
    return true;
  }

  size_t total_remainder = size - tail_remainder;
  LF_MemBlock *cur_block = slice->tail_block->next;
  while (cur_block) {
    if (size <= total_remainder) {
      slice->tail_block = cur_block;
      slice->tail_offset = total_remainder;
      slice->size += size;

      return true;
    }

    slice->size += cur_block->size;
    total_remainder -= cur_block->size;
    cur_block = cur_block->next;
  }

  assert(false && "reached end of arena while growing slice");
}
