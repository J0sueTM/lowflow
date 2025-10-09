#ifndef LF_STACK_H
#define LF_STACK_H

#include "./arena.h"

typedef struct LF_Stack {
  LF_Arena arena;
  size_t elem_qtt_in_block;
  size_t elem_size;
  size_t elem_alignment;
  size_t elem_padded_size;
  size_t elem_count;
} LF_Stack;

void lf_init_stack(LF_Stack *stack);

char *lf_alloc_stack_elem(LF_Stack *stack);

void lf_reset_stack(LF_Stack *stack);

char *lf_pop_from_stack(LF_Stack *stack);

#endif
