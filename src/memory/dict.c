#include "./dict.h"

void lf_init_dict(LF_Dict *dict) {
  assert(dict);
  assert(dict->key_block_size > 0);
  assert(dict->val_block_size > 0);

  dict->elem_count = 0;
  dict->keys_arena.block_size = dict->key_block_size;
  lf_init_arena(&dict->keys_arena);
  dict->vals_arena.block_size = dict->val_block_size;
  lf_init_arena(&dict->vals_arena);
}
