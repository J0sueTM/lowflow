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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "./types.h"
#include "./arena.h"
#include "./superhash.h"
#include "./hashdict.h"

int main(void) {
  HashDict *hd = hashdict_alloc(0, 0);
  hashdict_free(hd);

  /*
  Arena *arena = arena_alloc(0);
  int *val = arena_malloc(arena, sizeof(int));
  *val = 10;
 
  ID *person_id = (ID *)arena_malloc(arena, sizeof(ID));
  person_id->spec.type = STRUCT;
  person_id->name = "Person";
  person_id->is_anon = false;

  Struct *person_struct = (Struct *)arena_malloc(arena, sizeof(Struct));
  person_struct->id = person_id;

  Dict *person_spec = &person_id->spec.named_types;
  person_spec->keys = (Value *)malloc(sizeof(Value) * 2);
  person_spec->vals = (Value *)malloc(sizeof(Value) * 2);

  strcpy(person_spec->keys[0].keyword, "age");
  person_spec->vals[0].spec.type = INT;

  strcpy(person_spec->keys[1].keyword, "name");
  person_spec->vals[1].spec.type = STR;

  person_id->value._struct = person_struct;

  person_struct->vals = (Value *)malloc(sizeof(Value) * 2);

  person_struct->vals[0]._int = 20;
  person_struct->vals[1].str = (char *)malloc(sizeof(char) * 5);
  strcpy(person_struct->vals[1].str, "Josué");

  printf(
    "Person{:age %lld, :name \"%s\"}\n",
    person_struct->vals[0]._int,
    person_struct->vals[1].str
  );
  */

  ///////

  /*
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
    "%s :: (%s: %d, %s: %d) %d \\> ...\n",
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

  arena_free(arena);

  char *hello = "hello";
  char *bye = "bye";
  printf(
    "hello = %lu, bye = %lu\n",
    superhash(hello, strlen(hello)),
    superhash(bye, strlen(bye))
  );
 
  */

  return 0;
}
