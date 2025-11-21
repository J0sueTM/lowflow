#ifndef LF_LIST_H
#define LF_LIST_H

#include "./arena.h"

typedef struct LF_List {
  LF_Arena arena;
  size_t elem_qtt_in_block;
  size_t elem_size;
  size_t elem_alignment;
  size_t elem_padded_size;
  size_t elem_count;
} LF_List;

void lf_init_list(LF_List *list);

char *lf_alloc_list_elems(LF_List *list, size_t qtt);

char *lf_alloc_list_elem(LF_List *list);

void lf_reset_list(LF_List *list);

char *lf_get_next_list_elem(LF_List *list, char *elem);

char *lf_get_prev_list_elem(LF_List *list, char *elem);

char *lf_get_first_list_elem(LF_List *list);

char *lf_get_last_list_elem(LF_List *list);

char *lf_get_list_elem_by_pos(LF_List *list, size_t pos);

#endif
