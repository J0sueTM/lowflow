#include "./string.h"

void lf_init_string(LF_String *str) {
  assert(str);
  assert(str->arena);

  if (str->slice) {
    lf_init_arena_slice(str->slice, str->arena);
    return;
  }

  str->arena->block_size = str->char_qtt_in_block;
  str->arena->block_offset = LF_MEMBLOCK_OFFSET_LEFT;
  lf_init_arena(str->arena);
}

void lf_init_string_from_slice(LF_String *str, LF_ArenaSlice *slice) {
  assert(str);
  assert(str->arena);
  assert(slice);

  lf_init_arena_slice(slice, NULL);
  str->slice = slice;
  str->char_qtt_in_block = slice->head_block->size;
}

char *lf_alloc_string(LF_String *str, size_t size) {
  assert(str);

  char *str_ptr = NULL;
  if (str->slice) {
    str_ptr = lf_arena_slice_alloc_at_tail(str->slice, str->arena, size);
  } else {
    str_ptr = lf_arena_alloc_at_tail(str->arena, size);
  }
  assert(str_ptr);

  return str_ptr;
}

void lf_reset_string(LF_String *str) {
  assert(!str->slice && "cannot reset string, not owner");

  lf_reset_arena(str->arena);
}

size_t lf_get_string_size(LF_String *str) {
  if (str->slice) {
    return str->slice->size;
  }

  return str->arena->size;
}

char *lf_init_string_iteration(LF_StringIteration *iter) {
  assert(iter);
  assert(iter->str);

  iter->cur_block = (iter->str->slice) ? iter->str->slice->head_block
                                       : iter->str->arena->head_block;
  iter->block_offset = (iter->str->slice)
                         ? iter->str->slice->head_offset
                         : iter->str->arena->head_block->left_offset;

  return iter->cur_block->data + iter->block_offset;
}

char *lf_iterate_string(LF_StringIteration *iter) {
  assert(iter);
  assert(iter->str);

  if (!iter->cur_block) {
    return NULL;
  }

  size_t new_offset = iter->block_offset + 1;

  LF_MemBlock *last_block = (iter->str->slice) ? iter->str->slice->tail_block
                                               : iter->str->arena->tail_block;
  size_t right_offset = (iter->str->slice)
                          ? iter->str->slice->tail_offset
                          : iter->str->arena->tail_block->right_offset;
  bool reached_block_end = new_offset >= right_offset;
  if (iter->cur_block == last_block && reached_block_end) {
    iter->cur_block = NULL;
    return NULL;
  } else if (reached_block_end) {
    iter->cur_block = iter->cur_block->next;
    iter->block_offset = iter->cur_block->left_offset;
  } else {
    iter->block_offset = new_offset;
  }

  return iter->cur_block->data + iter->block_offset;
}

bool lf_compare_string(LF_StringComparison *comp) {
  assert(comp);

  LF_MemBlock *cur_block = (comp->left->slice) ? comp->left->slice->head_block
                                               : comp->left->arena->head_block;
  size_t left_str_size = lf_get_string_size(comp->left);

  switch (comp->type) {
    case LF_STR_COMPARISON_STR_CHAR: {
      assert(comp->right_char);

      if (left_str_size != comp->right_char_size) {
        return false;
      }

      size_t compared = 0;
      size_t remaining = (comp->right_char_size < left_str_size)
                           ? comp->right_char_size
                           : left_str_size;
      while (cur_block && remaining >= 1) {
        size_t left_offset;
        size_t right_offset;
        if (!comp->left->slice) {
          left_offset = cur_block->left_offset;
          right_offset = cur_block->right_offset;
        } else {
          if (cur_block == comp->left->slice->head_block) {
            left_offset = comp->left->slice->head_offset;
          }

          if (cur_block == comp->left->slice->tail_block) {
            right_offset = comp->left->slice->tail_offset;
          }
        }

        size_t right_padding = cur_block->size - right_offset;
        size_t used_size = cur_block->size - right_padding - left_offset;
        size_t comp_size = (used_size > remaining) ? remaining : used_size;

        // TODO: Find out how this API would/should be used to understand
        // what type of result should come up.
        int diff = memcmp(cur_block->data + left_offset,
                          comp->right_char + compared,
                          comp_size);
        if (diff != 0) {
          return false;
        }

        compared += comp_size;
        remaining -= comp_size;

        cur_block = cur_block->next;
      }
    } break;
    case LF_STR_COMPARISON_STR_STR:
      assert(false && "not implemented yet.");
      break;
  }

  return true;
}
