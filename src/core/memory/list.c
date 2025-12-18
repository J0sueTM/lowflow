#include "./list.h"

void lf_init_list(LF_List *list) {
  assert(list);
  assert(list->elem_size > 0);
  assert(list->elem_qtt_in_block > 0);

  list->elem_count = 0;
  size_t padding =
    (list->elem_alignment - (list->elem_size % list->elem_alignment)) %
    list->elem_alignment;
  list->elem_padded_size = list->elem_size + padding;

  list->arena.block_size = list->elem_qtt_in_block * list->elem_padded_size;
  list->arena.block_offset = LF_MEMBLOCK_OFFSET_MIDDLE;
  lf_init_arena(&list->arena);
}

char *lf_alloc_list_elems(LF_List *list, size_t qtt) {
  assert(list);
  assert(qtt > 0);

  size_t total_size = qtt * list->elem_size;
  char *elem = lf_arena_alloc_at_tail(&list->arena, total_size);
  list->elem_count += qtt;

  return elem;
}

char *lf_alloc_list_elem(LF_List *list) { return lf_alloc_list_elems(list, 1); }

void lf_reset_list(LF_List *list) {
  assert(list);

  lf_reset_arena(&list->arena);
}

char *lf_get_next_list_elem(LF_List *list, char *elem) {
  assert(list);
  assert(elem);

  LF_Arena *arena = &list->arena;

  char *cursor_block_start =
    arena->cursor_block->data + arena->cursor_block->left_offset;
  char *cursor_block_end =
    arena->cursor_block->data + arena->cursor_block->right_offset;
  bool is_elem_in_cursor_block =
    cursor_block_start <= elem && elem < cursor_block_end;

  LF_MemBlock *cur_block =
    (is_elem_in_cursor_block) ? arena->cursor_block : arena->head_block;
  for (size_t i = 0; i < arena->block_count; ++i) {
    char *last_elem =
      cur_block->data + cur_block->right_offset - list->elem_padded_size;
    char *first_elem = cur_block->data + cur_block->left_offset;
    if (first_elem <= elem && last_elem >= elem) {
      break;
    }
    cur_block = cur_block->next;
  }
  if (!cur_block) {
    return NULL;
  }

  arena->cursor_block = cur_block;

  bool is_last_elem =
    elem == cur_block->data + cur_block->right_offset - list->elem_padded_size;
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

  char *cursor_block_start =
    arena->cursor_block->data + arena->cursor_block->left_offset;
  char *cursor_block_end =
    arena->cursor_block->data + arena->cursor_block->right_offset;
  bool is_elem_in_cursor_block =
    cursor_block_start <= elem && elem < cursor_block_end;

  LF_MemBlock *cur_block =
    (is_elem_in_cursor_block) ? arena->cursor_block : arena->head_block;
  for (size_t i = 0; i < arena->block_count; ++i) {
    char *last_elem =
      cur_block->data + cur_block->right_offset - list->elem_padded_size;
    char *first_elem = cur_block->data + cur_block->left_offset;
    if (first_elem <= elem && elem <= last_elem) {
      break;
    }
    cur_block = cur_block->next;
  }

  if (!cur_block) {
    return NULL;
  }

  arena->cursor_block = cur_block;

  bool is_first_elem = elem == cur_block->data;
  if (!is_first_elem) {
    return elem - list->elem_padded_size;
  }

  LF_MemBlock *prev_block = cur_block->prev;
  if (!prev_block) {
    return NULL;
  }
  arena->cursor_block = prev_block;

  char *last_elem =
    prev_block->data + prev_block->right_offset - list->elem_padded_size;
  return last_elem;
}

char *lf_get_first_list_elem(LF_List *list) {
  assert(list);

  if (list->elem_count <= 0) {
    return NULL;
  }

  list->arena.cursor_block = list->arena.head_block;
  return list->arena.head_block->data + list->arena.head_block->left_offset;
}

char *lf_get_last_list_elem(LF_List *list) {
  assert(list);

  if (list->elem_count <= 0) {
    return NULL;
  }

  list->arena.cursor_block = list->arena.tail_block;
  return list->arena.tail_block->data + list->arena.tail_block->right_offset -
         list->elem_padded_size;
}
