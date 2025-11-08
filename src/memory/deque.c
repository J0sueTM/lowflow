#include "./deque.h"

void lf_init_deque(LF_Deque *deque) {
  assert(deque);
  assert(deque->elem_size > 0);
  assert(deque->elem_qtt_in_block > 0);

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

  deque->elem_count = 0;
  deque->head_read_offset = deque->arena.block_size;
  deque->arena.head_block->offset = deque->arena.block_size;
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
    ++deque->elem_count;
    return elem;
  }
 
  LF_MemBlock *new_head_block = lf_alloc_arena_memblock(arena);
  new_head_block->next = head_block;
  if (!arena->tail_block) {
    arena->tail_block = arena->head_block;
  }
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

  bool is_head_block_empty = deque->head_read_offset >= deque->arena.block_size;
  if (is_head_block_empty) {
    lf_dealloc_arena_head_memblock(&deque->arena);
    deque->head_read_offset = 0;
  }
  
  LF_MemBlock *head_block = deque->arena.head_block;
  char *elem = head_block->data + deque->head_read_offset;
  deque->head_read_offset += deque->elem_padded_size;
  --deque->elem_count;

  // If the remaining head block is empty, which would only happen if
  // all elements were removed, then we should move the offset all the
  // way to the end of the block, so further head pushes continue
  // going left.
  if (deque->elem_count <= 0) {
    deque->arena.head_block->offset = deque->arena.block_size;
    deque->head_read_offset = deque->arena.block_size;
  }
  
  return elem;
}

char *lf_pop_tail_elem_from_deque(LF_Deque *deque) {
  assert(deque);

  if (deque->elem_count <= 0) {
    return NULL;
  }

  LF_MemBlock *tail_block = deque->arena.tail_block;
  bool is_tail_block_empty = tail_block->offset <= 0;
  if (is_tail_block_empty && deque->arena.block_count >= 1) {
    tail_block = lf_dealloc_arena_tail_memblock(&deque->arena);
  }

  size_t elem_offset = tail_block->offset - deque->elem_padded_size;
  char *elem = tail_block->data + elem_offset;
  --deque->elem_count;
  tail_block->offset = elem_offset;
  return elem;
}
