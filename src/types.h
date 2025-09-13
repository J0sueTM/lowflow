#ifndef LF_TYPES_H
#define LF_TYPES_H

#include <stdbool.h>

typedef enum LF_Type {
  LF_INT, LF_FLOAT, LF_CHAR, LF_STR, LF_BOOL,
  LF_LIST, LF_SEQ, LF_DICT, LF_FUNC,
  LF_TYPE, LF_TRAIT
} LF_Type;

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
  LF_Type ret_type;

  void (*native_impl)(LF_Value *out, LF_Value *args, LF_Value *bindings);
} LF_FuncDefSpec;

typedef struct LF_FuncCallSpec {
  LF_Value *args;

  LF_Value *bindings;
  char **binding_names;
} LF_FuncCallSpec;

typedef struct LF_TraitSpec {
  LF_FuncDefSpec *func_def_specs;
} LF_TraitSpec;

typedef struct LF_Value {
  char *name;

  union {
    LF_Type type;
    LF_ListSpec *list_spec;
    LF_SeqSpec *seq_spec;
    LF_DictSpec *dict_spec;
    LF_FuncDefSpec *func_def_spec;
    LF_TraitSpec *trait_spec;
    LF_FuncCallSpec *func_call_spec;
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
