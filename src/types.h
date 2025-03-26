#ifndef LF_TYPES_H
#define LF_TYPES_H

#include <stdbool.h>

// Null doesn't exist in LowFlow.
typedef enum Type {
  STR, INT, FLOAT, BOOL,
  FUNC, KEYWORD, LIST,
  MAP, SEQ, TYPE, STRUCT
} Type;

typedef struct ID ID;
typedef union Value Value;
typedef struct Dict Dict;

typedef struct Dict {
  ID *id;
  Value *keys;
  Value *vals;
} Dict;

typedef union Spec {
  Type type;
  Dict named_types;
} Spec;

typedef struct List {
  ID *id;
  Value *vals;
} List;

typedef struct Seq {
  ID *id;
  Value *vals;
} Seq;

typedef struct Struct {
  ID *id;
  Value *vals;
} Struct;

typedef union Value {
  Spec spec; // A type can also be a value.
  char keyword[64];
  long long _int;
  double _float;
  char *str;
  List *list;
  Dict *map;
  Seq *seq;
  Struct *_struct;
} Value;

typedef struct ID {
  Spec spec; // List, Seq, Dict, Str, Int, etc.
  bool is_anon;
  Spec *minor_specs; // {Str, Int}, {Int}, etc.
  char *name; // my-data, my-func2
  Value value;
} ID;

#endif // LF_TYPES_H
