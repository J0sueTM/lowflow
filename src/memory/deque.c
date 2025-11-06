#include "./deque.h"

void lf_init_deque(LF_Deque *deque) {
  assert(deque);
  assert(deque->elem_size > 0);
  assert(deque->elem_qtt_in_block > 0);

  deque->elem_count = 0;
  deque->head_read_offset = 0;

  // TODO: This repeats in almost all data structures initializations.
  // Maybe create a helper?
  size_t padding = (
    (deque->elem_alignment - (deque->elem_size % deque->elem_alignment))
    % deque->elem_alignment
  );
  deque->elem_padded_size = deque->elem_size + padding;

  deque->arena.block_size = (
    deque->elem_qtt_in_block * deque->elem_padded_size
  );
  lf_init_arena(&deque->arena);
}

char *lf_alloc_deque_head_elem(LF_Deque *deque) {
  assert(deque);
 
  bool is_head_full = deque->head_read_offset <= 0;
  LF_Arena *arena = &deque->arena;
  LF_MemBlock *head_block = arena->head_block;
  if (!is_head_full) {
    size_t new_read_offset = deque->head_read_offset - deque->elem_padded_size;
    char *elem = head_block->data + new_read_offset;
    deque->head_read_offset = new_read_offset;
    return elem;
  }
 
  LF_MemBlock *new_head_block = lf_alloc_arena_memblock(arena);
  new_head_block->next = head_block;
  arena->tail_block = arena->head_block;
  arena->head_block = new_head_block;

  new_head_block->offset = arena->block_size;
  deque->head_read_offset = ((deque->elem_qtt_in_block - 1) * deque->elem_padded_size);
  char *elem = new_head_block->data + deque->head_read_offset;
  ++deque->elem_count;

  return elem;
}

char *lf_alloc_deque_tail_elem(LF_Deque *deque) {
  assert(deque);

  char *elem = lf_arena_alloc(&deque->arena, deque->elem_padded_size);
  ++deque->elem_count;
  return elem;
}

char *lf_pop_head_elem_from_deque(LF_Deque *deque) {
  assert(deque);

  if (deque->elem_count <= 0) {
    return NULL;
  }

  LF_MemBlock *head_block = deque->arena.head_block;
  char *elem = head_block->data + deque->head_read_offset;
  // TODO: Should we free the memblock if the read offset goes all the
  // way up? Or should we keep it for later? Maybe keep at least N blocks.
  // Think about it later.
  head_block->offset += deque->elem_padded_size;
  --deque->elem_count;

  return elem;
}
