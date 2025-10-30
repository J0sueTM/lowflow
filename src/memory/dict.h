#ifndef LF_DICT_H
#define LF_DICT_H

#include "./arena.h"

// TODO: DictKey and DictVal

typedef struct LF_Dict {
  LF_Arena keys_arena;
  LF_Arena vals_arena;
  size_t key_block_size;
  size_t val_block_size;
  size_t elem_count;
} LF_Dict;

void lf_init_dict(LF_Dict *dict);

#endif // lf-dict-h
