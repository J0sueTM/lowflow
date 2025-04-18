/*
 * Copyright (C) Josué Teodoro Moreira
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef LF_TYPES_H
#define LF_TYPES_H

#include <stdbool.h>

#include "./hashdict.h"

// Null doesn't exist in LowFlow.
#define TYPE_COUNT 11
typedef enum Type {
  // Primitives.
  STR = 0, INT = 1, FLOAT = 2, BOOL = 3, KEYWORD = 4,
  // Data structures.
  LIST = 5, DICT = 6, SEQ = 7, STRUCT = 8,
  // Higher order.
  FUNC = 9, TYPE = 10
} Type;

typedef struct ID ID;

// Primitives.

// These types are for language constructs only.
typedef char *Str;
typedef long long Int;
typedef double Float;
typedef bool Bool;
typedef char *Keyword;

// Data structures.
typedef struct List {
  ID *id;
  Arena *vals_arena;
} List;

typedef struct Dict {
  ID *id;
  HashDict *val_by_key;
} Dict;

typedef struct Seq {
  ID *id;
  Arena *vals_arena;
} Seq;

typedef struct Struct {
  ID *id;
  HashDict *vals_by_key;
} Struct;

typedef union Value Value;

// Higher Order
typedef struct Func {
  ID *id;
  Value *(*native_impl)(HashDict *arg_by_name,
                        Value *flowing_val);
  Value *child_func;
} Func;

typedef union Spec {
  Type type;
  HashDict *type_by_name;
} Spec;

typedef union Value {
  Str str;
  Int _int;
  Float _float;
  Bool _bool;
  Keyword keyword;
  List *list;
  Dict *dict;
  Seq *seq;
  Struct *_struct;
  Func *func;
  Spec spec; // A type can also be a value.
} Value;

typedef struct ID {
  Spec spec; // List, Seq, Dict, Str, Int, etc.
  bool is_anon;
  Spec *minor_specs; // {Str, Int}, {Int}, etc.
  char *name; // my-data, my-func2
  Value val;
} ID;

typedef struct Module {
  Arena *ids_arena;
  HashDict *id_by_name;

  Arena *minor_specs_arena;

  Arena *funcs_arena;
  Arena *func_arg_type_by_name_hds_arena;
} Module;

typedef struct Flow {
  ID *ids;
  Value val;

  Arena *flowing_vals_arena;
} Flow;

typedef struct Cluster {
  Arena *flows_arena;
} Cluster;

#endif // LF_TYPES_H
