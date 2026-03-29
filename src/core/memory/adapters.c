#include "./adapters.h"

// Offsets are calculated based on their distance from the middle of
// the entire arena. For now, assumes every block has the same size.
static void _lf_get_relative_offset(LF_Arena *arena,
                                    size_t elem_size,
                                    ssize_t *left_offset,
                                    ssize_t *right_offset) {
  bool has_no_elems = arena->head_block->left_offset == arena->head_block->right_offset;
  if (arena->block_count == 1 && has_no_elems) {
    *left_offset = 0;
    *right_offset = 0;
    return;
  }

  size_t arena_size = arena->block_count * arena->block_size;
  size_t middle_offset = arena_size / 2;

  size_t left_distance = middle_offset - arena->head_block->left_offset;

  size_t total_right_offset =
    arena_size - arena->block_size + arena->tail_block->right_offset;
  size_t right_distance = total_right_offset - middle_offset;
  assert(left_distance % elem_size == 0);
  assert(right_distance % elem_size == 0);

  *left_offset = left_distance / elem_size;
  *right_offset = right_distance / elem_size;
}

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

void lf_init_arena_slice(LF_Arena *src, LF_ArenaSlice *dest) {
  assert(src);
  assert(dest);

  _lf_get_relative_offset(src,
                          dest->elem_size,
                          &dest->left_offset,
                          &dest->right_offset);

  dest->head_block = src->head_block;
  dest->tail_block = src->tail_block;
}

bool lf_init_arena_slice_with_relative_offsets(LF_Arena *src,
                                               LF_ArenaSlice *dest,
                                               ssize_t left_offset,
                                               ssize_t right_offset) {
  assert(src);
  assert(dest);

  ssize_t total_left_offset, total_right_offset;
  _lf_get_relative_offset(src,
                          dest->elem_size,
                          &total_left_offset,
                          &total_right_offset);

  bool is_left_offset_outbound = false;
  bool is_right_offset_outbound = false;

  bool is_left_offset_skewed = left_offset < 0;
  bool is_right_offset_skewed = right_offset < 0;
  bool is_any_offset_skewed = is_left_offset_outbound || is_right_offset_skewed;
  if (is_any_offset_skewed && right_offset < left_offset) {
    return false;
  }

  if (is_left_offset_skewed) {
    size_t deskewed_left_offset = left_offset * -1;
    is_left_offset_outbound = deskewed_left_offset > total_right_offset;
  } else {
    is_left_offset_outbound = left_offset > total_left_offset;
  }

  if (is_right_offset_skewed) {
    size_t deskewed_right_offset = right_offset * -1;
    is_right_offset_outbound = deskewed_right_offset > total_left_offset;
  } else {
    is_right_offset_outbound = right_offset > total_right_offset;
  }

  bool is_outbound = is_left_offset_outbound || is_right_offset_outbound;
  if (is_outbound) {
    return false;
  }

  dest->left_offset = left_offset;
  dest->right_offset = right_offset;

  size_t arena_size = src->block_size * src->block_count;
  size_t middle_offset = arena_size / 2;

  size_t abs_left_offset = middle_offset - left_offset * dest->elem_size;
  size_t head_block_end_offset = abs_left_offset + (abs_left_offset % src->block_size);
  size_t head_block_pos = (head_block_end_offset / src->block_size) - 1;

  size_t abs_right_offset = middle_offset + (right_offset * dest->elem_size);
  size_t tail_block_end_offset = abs_right_offset + (abs_right_offset % src->block_size);
  size_t tail_block_pos = (tail_block_end_offset / src->block_size) - 1;

  dest->head_block = NULL;
  dest->tail_block = NULL;
  LF_MemBlock *cur_block = src->head_block;
  for (size_t i = 0; i < src->block_count; ++i) {
    if (i == head_block_pos) {
      dest->head_block = cur_block;
    }
    if (i == tail_block_pos) {
      dest->tail_block = cur_block;
    }

    cur_block = cur_block->next;
  }

  assert(dest->head_block != NULL);
  assert(dest->tail_block != NULL);

  return true;
}

char *lf_arena_slice_alloc_at_head(LF_ArenaSlice *slice,
                                   LF_Arena *arena,
                                   size_t size) {
  assert(slice);
  assert(arena);

  if (slice->head_block != arena->head_block) {
    return NULL;
  }

  ssize_t total_left_offset, total_right_offset;
  _lf_get_relative_offset(arena,
                          slice->elem_size,
                          &total_left_offset,
                          &total_right_offset);
  bool lost_head = slice->left_offset < total_left_offset;
  if (lost_head) {
    return NULL;
  }

  char *ptr = lf_arena_alloc_at_head(arena, size);
  assert(ptr);
  _lf_get_relative_offset(arena,
                          slice->elem_size,
                          &slice->left_offset,
                          &total_right_offset);
  slice->head_block = arena->head_block;

  return ptr;
}

char *lf_arena_slice_alloc_at_tail(LF_ArenaSlice *slice,
                                   LF_Arena *arena,
                                   size_t size) {
  assert(slice);
  assert(arena);

  if (slice->tail_block != arena->tail_block) {
    return NULL;
  }

  ssize_t total_left_offset, total_right_offset;
  _lf_get_relative_offset(arena,
                          slice->elem_size,
                          &total_left_offset,
                          &total_right_offset);
  bool lost_tail = total_right_offset > slice->right_offset;
  if (lost_tail) {
    return NULL;
  }

  char *ptr = lf_arena_alloc_at_tail(arena, size);
  assert(ptr);
  _lf_get_relative_offset(arena,
                          slice->elem_size,
                          &total_left_offset,
                          &slice->right_offset);
  slice->tail_block = arena->tail_block;

  return ptr;
}
