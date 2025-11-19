#include "./string.h"

void lf_init_string(LF_String *str) {
  assert(str);
  assert(str->char_qtt_in_block > 0);
  str->arena.block_size = str->char_qtt_in_block;
  lf_init_arena(&str->arena);
}

char *lf_alloc_string(LF_String *str, size_t size) {
  char *str_ptr = lf_arena_alloc(&str->arena, size);
  ++str->str_qtt;
  return str_ptr;
}

void lf_reset_string(LF_String *str) {
  lf_reset_arena(&str->arena);
  str->str_qtt = 0;
}
