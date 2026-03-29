#ifndef LF_CORE_MEMORY_ADAPTERS
#define LF_CORE_MEMORY_ADAPTERS

#include "./list.h"
#include "./stack.h"
#include "./arena.h"

void lf_stack_to_list(LF_Stack *stack, LF_List *list);

typedef struct LF_ArenaSlice {
  LF_MemBlock *head_block;
  LF_MemBlock *tail_block;
  size_t head_offset;
  size_t tail_offset;

  size_t elem_size;
} LF_ArenaSlice;

void lf_init_arena_slice(LF_ArenaSlice *dest, LF_Arena *src);

char *lf_arena_slice_alloc_at_head(LF_ArenaSlice *slice,
                                   LF_Arena *arena,
                                   size_t size);

char *lf_arena_slice_alloc_at_tail(LF_ArenaSlice *slice,
                                   LF_Arena *arena,
                                   size_t size);

#endif // LF_CORE_MEMORY_ADAPTERS
