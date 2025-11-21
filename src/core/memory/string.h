#ifndef LF_STRING_H
#define LF_STRING_H

#include "arena.h"

// TODO: Maybe make so strings can be broken between blocks.
typedef struct LF_String {
  LF_Arena arena;
  size_t char_qtt_in_block;
  size_t str_qtt;
} LF_String;

void lf_init_string(LF_String *str);

char *lf_alloc_string(LF_String *str, size_t size);

void lf_reset_string(LF_String *str);

char *lf_get_string_start(LF_String *str);

char *lf_string_to_cstr(LF_String *str);

#endif // LF_STRING_H
