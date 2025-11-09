#ifndef LF_DEQUEUE_H
#define LF_DEQUEUE_H

#include "./arena.h"

typedef struct LF_Deque {
  LF_Arena arena;
  size_t elem_qtt_in_block;
  size_t elem_size;
  size_t elem_alignment;
  size_t elem_padded_size;
  size_t elem_count;
  size_t head_read_offset;
} LF_Deque;

void lf_init_deque(LF_Deque *deque);

char *lf_alloc_deque_head_elem(LF_Deque *deque);

char *lf_alloc_deque_tail_elem(LF_Deque *deque);

char *lf_pop_head_elem_from_deque(LF_Deque *deque);

char *lf_pop_tail_elem_from_deque(LF_Deque *deque);

#endif
