#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

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
  // NOTE: The keys are in the Spec. If anon, they can be defined here.
  Value *keys;
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

int main(void) {
  ID person_id;
  person_id.spec.type = STRUCT;
  person_id.name = "Person";
  person_id.is_anon = false;

  Struct person_struct;
  person_struct.id = &person_id;

  Dict *person_spec = &person_id.spec.named_types;
  person_spec->keys = (Value *)malloc(sizeof(Value) * 2);
  person_spec->vals = (Value *)malloc(sizeof(Value) * 2);

  strcpy(person_spec->keys[0].keyword, "age");
  person_spec->vals[0].spec.type = INT;

  strcpy(person_spec->keys[1].keyword, "name");
  person_spec->vals[1].spec.type = STR;

  person_id.value._struct = &person_struct;

  person_struct.vals = (Value *)malloc(sizeof(Value) * 2);

  person_struct.vals[0]._int = 20;
  person_struct.vals[1].str = (char *)malloc(sizeof(char) * 5);
  strcpy(person_struct.vals[1].str, "Josué");

  printf(
    "Person{:age %lld, :name \"%s\"}\n",
    person_struct.vals[0]._int,
    person_struct.vals[1].str
  );

  ///////

  ID add_id;
  add_id.spec.type = FUNC;
  add_id.name = "+";
  // args and return type
  add_id.minor_specs = (Spec *)malloc(sizeof(Spec) * 2);

  Dict *add_args = &add_id.minor_specs[0].named_types;
  add_args->keys = (Value *)malloc(sizeof(Value) * 2);
  add_args->vals = (Value *)malloc(sizeof(Value) * 2);

  add_args->keys[0].str = (char *)malloc(sizeof(char) * 4);
  strcpy(add_args->keys[0].str, "fst");
  add_args->vals[0].spec.type = INT;

  add_args->keys[1].str = (char *)malloc(sizeof(char) * 4);
  strcpy(add_args->keys[1].str, "snd");
  add_args->vals[1].spec.type = INT;

  add_id.minor_specs[2].type = INT;

  printf(
    "%s :: (%s: %d, %s: %d) %d \\> ...",
    add_id.name,
    // fst
    add_id.minor_specs[0].named_types.keys[0].str,
    add_id.minor_specs[0].named_types.vals[0].spec.type,
    // snd
    add_id.minor_specs[0].named_types.keys[1].str,
    add_id.minor_specs[0].named_types.vals[1].spec.type,
    // return
    add_id.minor_specs[1].type = INT
  );
  
  return 0;
}
