#include "./list.h"

void lf_init_list(LF_List *list) {
  assert(list);
  assert(list->elem_size > 0);
  assert(list->elem_qtt_in_block > 0);

  list->elem_count = 0;
  size_t padding = (
    (list->elem_alignment - (list->elem_size % list->elem_alignment))
    % list->elem_alignment
  );
  list->elem_padded_size = list->elem_size + padding;

  list->arena.block_size = (
    list->elem_qtt_in_block * list->elem_padded_size
  );
  lf_init_arena(&list->arena);
}

char *lf_alloc_list_elems(LF_List *list, size_t qtt) {
  assert(list);
  assert(qtt > 0);

  size_t total_size = qtt * list->elem_size;
  char *elem = lf_arena_alloc(&list->arena, total_size);
  list->elem_count += qtt;

  return elem;
}

char *lf_alloc_list_elem(LF_List *list) {
  return lf_alloc_list_elems(list, 1);
}

void lf_reset_list(LF_List *list) {
  assert(list);

  lf_reset_arena(&list->arena);
}

char *lf_get_next_list_elem(LF_List *list, char *elem) {
  assert(list);
  assert(elem);

  LF_Arena *arena = &list->arena;

  char *cursor_block_start = arena->cursor_block->data;
  char *cursor_block_end = (
    arena->cursor_block->data + (arena->block_size - 1)
  );
  bool is_elem_in_cursor_block = (
    cursor_block_start <= elem && elem <= cursor_block_end
  );

  bool is_last_elem = false;
  LF_MemBlock *cur_block = (
    (is_elem_in_cursor_block)
      ? arena->cursor_block
      : arena->head_block
  );
  for (size_t i = 0; i < arena->block_count; ++i) {
    char *last_elem = (
      cur_block->data
        + ((list->elem_qtt_in_block - 1)
        * list->elem_padded_size)
    );
    char *first_elem = cur_block->data;
    if (first_elem <= elem && last_elem >= elem) {
      is_last_elem = elem >= last_elem;
      break;
    }
    arena->cursor_block = cur_block;
    cur_block = cur_block->next;
  }

  if (!cur_block) {
    return NULL;
  }

  const size_t full_offset = (
    elem + list->elem_padded_size - cur_block->data
  );
  if (full_offset >= cur_block->offset) {
    return NULL;
  }

  if (!is_last_elem) {
    return elem + list->elem_padded_size;
  }

  LF_MemBlock *next_block = cur_block->next;
  if (!next_block) {
    return NULL;
  }
  arena->cursor_block = next_block;

  return next_block->data;
}

char *lf_get_prev_list_elem(LF_List *list, char *elem) {
  assert(list);
  assert(elem);

  LF_Arena *arena = &list->arena;

  char *cursor_block_start = arena->cursor_block->data;
  char *cursor_block_end = (
    arena->cursor_block->data + (arena->block_size - 1)
  );
  bool is_elem_in_cursor_block = (
    cursor_block_start <= elem && elem <= cursor_block_end
  );

  LF_MemBlock *cur_block = (
    (is_elem_in_cursor_block)
      ? arena->cursor_block
      : arena->head_block
  );
  for (size_t i = 0; i < arena->block_count; ++i) {
    char *last_elem = (
      cur_block->data
        + ((list->elem_qtt_in_block - 1)
        * list->elem_padded_size)
    );
    char *first_elem = cur_block->data;
    if (first_elem <= elem && last_elem >= elem) {
      break;
    }
    arena->cursor_block = cur_block;
    cur_block = cur_block->next;
  }

  if (!cur_block) {
    return NULL;
  }

  bool is_first_elem = elem == cur_block->data;
  if (!is_first_elem) {
    return elem - list->elem_padded_size;
  }

  LF_MemBlock *prev_block = cur_block->prev;
  if (!prev_block) {
    return NULL;
  }
  arena->cursor_block = prev_block;

  char *last_elem = (
    prev_block->data
    + (list->elem_qtt_in_block -1)
    * list->elem_padded_size
  );
  return last_elem;
}

char *lf_get_first_list_elem(LF_List *list) {
  assert(list);

  if (list->elem_count <= 0) {
    return NULL;
  }

  list->arena.cursor_block = list->arena.head_block;
  return list->arena.head_block->data;
}

char *lf_get_last_list_elem(LF_List *list) {
  assert(list);

  // TODO: this calculation is wrong.
  // 500 % 2 = 0, but should be 2, because we want the second
  // elem.
  list->arena.cursor_block = list->arena.tail_block;
  size_t elem_qtt_in_last_block = (
    list->elem_qtt_in_block % list->elem_count
  );
  return (
    list->arena.tail_block->data
    + elem_qtt_in_last_block
    * list->elem_padded_size
  );
}

char *lf_get_list_elem_by_pos(LF_List *list, size_t pos) {
  assert(list);

  LF_Arena *arena = &list->arena;
  size_t block_pos = pos / list->elem_qtt_in_block;
  size_t elem_pos_in_block = pos % list->elem_qtt_in_block;

  // TODO: Optimization. Start from the top or the head depending
  // on distance.
  LF_MemBlock *cur_block = arena->head_block;
  for (size_t i = 0; i < block_pos; ++i) {
    cur_block = cur_block->next;
  }
  assert(cur_block);

  size_t offset = elem_pos_in_block * list->elem_padded_size;
  return (cur_block->data + offset);
}
