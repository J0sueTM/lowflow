#include "./arena.h"

static void _lf_set_block_offset(LF_Arena *arena, LF_MemBlock *block) {
  switch (arena->block_offset) {
    case LF_MEMBLOCK_OFFSET_LEFT: {
      block->left_offset = 0;
      block->right_offset = 0;
    } break;
    case LF_MEMBLOCK_OFFSET_RIGHT: {
      block->left_offset = arena->block_size;
      block->right_offset = arena->block_size;
    } break;
    case LF_MEMBLOCK_OFFSET_MIDDLE:
    default: {
      size_t block_middle = (size_t)(arena->block_size * 0.5f);
      block->left_offset = block_middle;
      block->right_offset = block_middle;
    } break;
  }
}

void lf_init_arena(LF_Arena *arena) {
  assert(arena);
  assert(arena->block_size);

  arena->block_count = 0;
  arena->head_block = NULL;
  arena->tail_block = NULL;

  lf_alloc_arena_head_block(arena);
  arena->tail_block = arena->head_block;
  _lf_set_block_offset(arena, arena->head_block);

  arena->cursor_block = arena->head_block;
}

LF_MemBlock *lf_alloc_arena_head_block(LF_Arena *arena) {
  assert(arena);
  assert(arena->block_size);

  LF_MemBlock *new_block = (LF_MemBlock *)malloc(sizeof(LF_MemBlock));
  assert(new_block);

  new_block->data = (char *)malloc(arena->block_size);
  assert(new_block->data);

  LF_MemBlock *old_head_block = arena->head_block;
  if (old_head_block) {
    old_head_block->prev = new_block;
    new_block->next = old_head_block;
  } else {
    new_block->next = NULL;
  }
  new_block->prev = NULL;
  new_block->right_offset = arena->block_size;
  new_block->left_offset = arena->block_size;
  new_block->size = arena->block_size;

  ++arena->block_count;

  arena->head_block = new_block;

  return new_block;
}

LF_MemBlock *lf_alloc_arena_tail_block(LF_Arena *arena) {
  assert(arena);
  assert(arena->block_size);

  LF_MemBlock *new_block = (LF_MemBlock *)malloc(sizeof(LF_MemBlock));
  assert(new_block);

  new_block->data = (char *)malloc(arena->block_size);
  assert(new_block->data);

  LF_MemBlock *old_tail_block = arena->tail_block;
  if (old_tail_block) {
    old_tail_block->next = new_block;
    new_block->prev = old_tail_block;
  } else {
    new_block->prev = NULL;
  }
  new_block->next = NULL;
  new_block->right_offset = 0;
  new_block->left_offset = 0;
  new_block->size = arena->block_size;

  ++arena->block_count;

  arena->tail_block = new_block;

  return new_block;
}

char *lf_arena_alloc_at_head(LF_Arena *arena, size_t size) {
  assert(arena);
  assert(size > 0);
  assert(size <= arena->block_size);

  LF_MemBlock *head_block = arena->head_block;
  size_t remaining_size = head_block->left_offset;
  if (remaining_size < size) {
    head_block = lf_alloc_arena_head_block(arena);
  }

  size_t filled_offset = head_block->left_offset - size;
  char *ptr = head_block->data + filled_offset;
  head_block->left_offset = filled_offset;
  return ptr;
}

char *lf_arena_alloc_at_tail(LF_Arena *arena, size_t size) {
  assert(arena);
  assert(size > 0);
  assert(size <= arena->block_size);

  LF_MemBlock *tail_block = arena->tail_block;
  size_t remaining_size = tail_block->size - tail_block->right_offset;
  if (remaining_size < size) {
    tail_block = lf_alloc_arena_tail_block(arena);
  }

  char *ptr = tail_block->data + tail_block->right_offset;

  size_t filled_offset = tail_block->right_offset + size;
  tail_block->right_offset = filled_offset;

  return ptr;
}

void lf_dealloc_arena_block(LF_Arena *arena, LF_MemBlock *block) {
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

LF_MemBlock *lf_dealloc_arena_head_block(LF_Arena *arena) {
  assert(arena);

  if (arena->block_count <= 1) {
    return arena->head_block;
  }

  LF_MemBlock *next_block = arena->head_block->next;
  assert(next_block);

  lf_dealloc_arena_block(arena, arena->head_block);
  arena->head_block = next_block;

  return next_block;
}

LF_MemBlock *lf_dealloc_arena_tail_block(LF_Arena *arena) {
  assert(arena);

  if (arena->block_count <= 1) {
    return arena->tail_block;
  }

  LF_MemBlock *prev_block = arena->tail_block->prev;
  assert(prev_block);

  lf_dealloc_arena_block(arena, arena->tail_block);
  arena->tail_block = prev_block;

  return prev_block;
}

// TODO: Retain allocated blocks for faster reuse, but this
// can lead to unbounded memory growth if a workload
// temporarily spikes. Consider adding a configurable cap:
// if the arena's total capacity exceeds the cap when reset,
// free excess blocks.
void lf_reset_arena(LF_Arena *arena) {
  assert(arena);

  LF_MemBlock *head_block = arena->head_block;
  _lf_set_block_offset(arena, arena->head_block);

  LF_MemBlock *cur_block = head_block->next;
  while (cur_block) {
    cur_block->left_offset = 0;
    cur_block->right_offset = 0;
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

// TODO: This implements a very naive O(n) search. It
// iterates over the entire arena, and compares bytes one by
// one. This is bad. There are countless optimizations that
// can be done. However, for now, since all we want is to
// get this compiler up and running, making everything top
// notch isn't a priority.
//
// Also, LowFlow is still a toy language at the time of this
// comment being written, so since it haven't seen any real
// world problems, we have no idea of what bottlenecks it
// could go through, and consequently which would be the
// best solution.
char *lf_get_arena_elem_by_content(LF_Arena *arena,
                                   char *content,
                                   size_t elem_size,
                                   size_t elem_padded_size) {
  assert(arena);
  assert(content);

  LF_MemBlock *cur_block = arena->cursor_block;
  LF_MemBlock *next_block = arena->head_block;
  while (cur_block) {
    bool is_block_empty = cur_block->left_offset == cur_block->right_offset;
    if (is_block_empty) {
      goto skip_block;
      continue;
    }

    size_t used_size = cur_block->right_offset - cur_block->left_offset;
    if (used_size < elem_padded_size) {
      goto skip_block;
      continue;
    }

    char *last_elem =
      cur_block->data + cur_block->right_offset - elem_padded_size;
    for (char *cur_char = cur_block->data + cur_block->left_offset;
         cur_char <= last_elem;
         cur_char += elem_padded_size) {
      // NOTE: We use == for pointer-sized elements and
      // memcmp for larger ones. Pointer equality (==) is
      // guaranteed by the C standard to correctly compare
      // whether two pointers refer to the same object,
      // regardless of how the pointer works internally.
      //
      // In contrast, memcmp() compares raw bytes, which
      // works for generic data but may fail on exotic
      // architectures where equal pointers don’t have
      // identical bit patterns (e.g., tagged or segmented
      // memory).
      bool is_byte_sized = elem_size == sizeof(void *);
      bool bytes_match =
        (is_byte_sized ? *(char **)cur_char == *(char **)content
                       : memcmp(cur_char, content, elem_size) == 0);
      if (bytes_match) {
        return cur_char;
      }
    }

  skip_block:
    if (next_block == arena->cursor_block) {
      next_block = arena->cursor_block->next;
    }
    cur_block = next_block;
    next_block = (cur_block) ? cur_block->next : NULL;
  }

  return NULL;
}
