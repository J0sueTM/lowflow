#ifndef LF_TYPES_H
#define LF_TYPES_H

#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stdlib.h>

#include "./memory/stack.h"

typedef enum LF_Type {
  LF_INT = 1 << 0,
  LF_FLOAT = 1 << 1,
  LF_CHAR = 1 << 2,
  LF_STR = 1 << 3,
  LF_BOOL = 1 << 4,
  LF_SYMBOL = 1 << 5,
  LF_LIST = 1 << 6,
  LF_SEQ = 1 << 7,
  LF_DICT = 1 << 8,
  LF_FUNC_DEF = 1 << 9,
  LF_FUNC_CALL = 1 << 10,
  LF_TYPE = 1 << 11,
  LF_TRAIT = 1 << 12
} LF_Type;

#define PRIMITIVE_MASK                                                      \
  (LF_INT | LF_FLOAT | LF_CHAR | LF_STR | LF_BOOL | LF_FUNC_DEF | LF_TYPE | \
   LF_TRAIT)

typedef struct LF_Value LF_Value;

typedef struct LF_ListSpec {
  LF_Type elem_type;
} LF_ListSpec;

typedef struct LF_SeqSpec {
  LF_Type *elem_types;
} LF_SeqSpec;

typedef struct LF_DictSpec {
  // TODO: Make into map
  LF_Type *val_types;
  char **keys;
} LF_DictSpec;

typedef struct LF_FuncDefSpec {
  // TODO: make into map
  LF_Type *arg_types;
  char **arg_names;
  size_t arg_qtt;
  LF_Type ret_type;

  void (*native_impl)(LF_Value *out, LF_Stack *vals);
} LF_FuncDefSpec;

typedef struct LF_FuncCallSpec {
  LF_Value *args;
} LF_FuncCallSpec;

typedef struct LF_TraitSpec {
  LF_FuncDefSpec *func_def_specs;
} LF_TraitSpec;

typedef struct LF_SymbolSpec {
  LF_Type type;
  size_t slot;
} LF_SymbolSpec;

typedef struct LF_Value {
  char *name;
  LF_Type type;

  union {
    LF_ListSpec *list_spec;
    LF_SeqSpec *seq_spec;
    LF_DictSpec *dict_spec;
    LF_FuncDefSpec *func_def_spec;
    LF_TraitSpec *trait_spec;
    LF_FuncCallSpec *func_call_spec;
    LF_SymbolSpec *symbol_spec;
  };

  union {
    int as_int;
    float as_float;
    char as_char;
    char *as_str;
    bool as_bool;
    char *as_byte_ptr;
    void *as_void_ptr;
    LF_Value *inner_val;
  };
} LF_Value;

#endif // LF_TYPES_H
