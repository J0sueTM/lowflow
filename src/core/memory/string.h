#ifndef LF_STRING_H
#define LF_STRING_H

#include "./adapters.h"
#include "./arena.h"

typedef struct LF_String {
  LF_Arena *arena;
  LF_ArenaSlice *slice;
  size_t char_qtt_in_block;
} LF_String;

typedef struct LF_StringIteration {
  LF_String *str;
  LF_MemBlock *cur_block;
  size_t block_offset;
} LF_StringIteration;

typedef enum LF_StringComparisonType {
  LF_STR_COMPARISON_STR_STR,
  LF_STR_COMPARISON_STR_CHAR,
} LF_StringComparisonType;

typedef struct LF_StringComparison {
  LF_StringComparisonType type;

  LF_String *left;
  union {
    LF_String *right_str;
    char *right_char;
  };

  size_t right_char_size;
} LF_StringComparison;

void lf_init_string(LF_String *str);

void lf_init_string_from_slice(LF_String *str, LF_ArenaSlice *slice);

char *lf_alloc_string(LF_String *str, size_t size);

void lf_reset_string(LF_String *str);

size_t lf_get_string_size(LF_String *str);

char *lf_init_string_iteration(LF_StringIteration *iter);

char *lf_iterate_string(LF_StringIteration *iter);

bool lf_compare_string(LF_StringComparison *comp);

#endif // LF_STRING_H
