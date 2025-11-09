#include "./arena.h"

void lf_init_arena(LF_Arena *arena) {
  assert(arena);
  assert(arena->block_size);

  arena->block_count = 0;
  arena->head_block = lf_alloc_arena_memblock(arena);
  arena->head_block->offset = 0;
  arena->head_block->prev = NULL;
  arena->head_block->next = NULL;

  arena->tail_block = arena->head_block;

  arena->cursor_block = arena->head_block;
}

char *lf_arena_alloc(LF_Arena *arena, size_t size) {
  assert(arena);
  assert(size > 0);

  LF_MemBlock *tail_block = arena->tail_block;
  size_t remaining_size = tail_block->size - tail_block->offset;
  if (size <= remaining_size) {
    size_t filled_offset = tail_block->offset + size;
    char *ptr = tail_block->data + tail_block->offset;
    tail_block->offset = filled_offset;
    return ptr;
  }

  LF_MemBlock *new_block = lf_alloc_arena_memblock(arena);
  new_block->offset = size;
  tail_block->next = new_block;
  new_block->prev = tail_block;
  arena->tail_block = new_block;

  return new_block->data;
}

LF_MemBlock *lf_alloc_arena_memblock(LF_Arena *arena) {
  assert(arena);

  LF_MemBlock *new_block = (LF_MemBlock *)malloc(sizeof(LF_MemBlock));
  assert(new_block);
  new_block->size = arena->block_size;
  new_block->next = NULL;
  new_block->offset = 0;
  ++arena->block_count;
  
  new_block->data = (char *)malloc(arena->block_size);
  assert(new_block->data);

  return new_block;
}

void lf_dealloc_arena_memblock(LF_Arena *arena, LF_MemBlock *block) {
  assert(block);

  LF_MemBlock *prev_block = block->prev;
  LF_MemBlock *next_block = block->next;

  if (prev_block) {
    prev_block->next = next_block;
  }

  if (next_block) {
    next_block->prev = prev_block;
  }

  --arena->block_count;

  free(block->data);
  free(block);
}

LF_MemBlock *lf_dealloc_arena_head_memblock(LF_Arena *arena) {
  assert(arena);

  if (arena->block_count <= 1) {
    return arena->head_block;
  }

  LF_MemBlock *next_block = arena->head_block->next;
  assert(next_block);

  lf_dealloc_arena_memblock(arena, arena->head_block);
  arena->head_block = next_block;

  return arena->head_block;
}

LF_MemBlock *lf_dealloc_arena_tail_memblock(LF_Arena *arena) {
  assert(arena);

  if (arena->block_count <= 1) {
    return arena->tail_block;
  }

  LF_MemBlock *prev_block = arena->tail_block->prev;
  assert(prev_block);

  lf_dealloc_arena_memblock(arena, arena->tail_block);
  arena->tail_block = prev_block;

  return arena->tail_block;
}

// TODO: Retain allocated blocks for faster reuse, but this can
// lead to unbounded memory growth if a workload temporarily spikes.
// Consider adding a configurable cap: if the arena's total capacity
// exceeds the cap when reset, free excess blocks.
void lf_reset_arena(LF_Arena *arena) {
  assert(arena);

  LF_MemBlock *cur_block = arena->head_block;
  while (cur_block) {
    cur_block->offset = 0;
    cur_block = cur_block->next;
  }
}

void lf_dealloc_arena(LF_Arena *arena) {
  assert(arena);

  LF_MemBlock *cur_block = arena->head_block;
  LF_MemBlock *next_block = NULL;
  while (cur_block) {
    next_block = cur_block->next;
    free(cur_block->data);
    free(cur_block);
    cur_block = next_block;
  }
}

// TODO: This implements a very naive O(n) search. It iterates over
// the entire arena, and compares bytes one by one. This is bad. There
// are countless optimizations that can be done. However, for now,
// since all we want is to get this compiler up and running, making
// everything top notch isn't a priority.
//
// Also, LowFlow is still a toy language at the time of this comment
// being written, so since it haven't seen any real world problems,
// we have no idea of what bottlenecks it could go through, and
// consequently which would be the best solution.
char *lf_get_arena_elem_by_content(
  LF_Arena *arena,
  char *content,
  size_t elem_size,
  size_t elem_padded_size
) {
  assert(arena);
  assert(content);

  LF_MemBlock *cur_block = arena->cursor_block;
  LF_MemBlock *next_block = arena->head_block;
  while (cur_block) {
    char *last_elem = cur_block->data + cur_block->offset;
    for (
      char *cur_char = cur_block->data;
      cur_char <= last_elem;
      cur_char += elem_padded_size
    ) {
      // NOTE: We use == for pointer-sized elements and memcmp for
      // larger ones. Pointer equality (==) is guaranteed by the C
      // standard to correctly compare whether two pointers refer to
      // the same object, regardless of how the pointer works internally.
      //
      // In contrast, memcmp() compares raw bytes, which works for
      // generic data but may fail on exotic architectures where equal
      // pointers don’t have identical bit patterns (e.g., tagged or
      // segmented memory).
      bool is_byte_sized = elem_size == sizeof(void *);
      bool bytes_match = (
        is_byte_sized
          ? *(char **)cur_char == *(char **)content
          : memcmp(cur_char, content, elem_size) == 0
      );
      if (bytes_match) {
        return cur_char;
      }
    }

    if (next_block == arena->cursor_block) {
      next_block = arena->cursor_block->next;
    }
    cur_block = next_block;
  }

  return NULL;
}
