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
#include "./murmurhash3.h"
#include "./hashdict.h"
#include "./func.h"

Value *sum_func_native_impl(HashDict *arg_by_name, Value *flowing_val) {
  HashDictEntry *fst_entry = hashdict_get_entry(arg_by_name, "fst", 3);
  HashDictEntry *snd_entry = hashdict_get_entry(arg_by_name, "snd", 3);

  Int fst = (Int)*fst_entry->val;
  Int snd = (Int)*snd_entry->val;

  flowing_val->_int = fst + snd;

  return flowing_val;
}

int main(void) {
  Type int_type = INT;

  // module
  Module math_mod = {
    .ids_arena = arena_alloc(10 * sizeof(ID)),
    .id_by_name = hashdict_alloc(NULL, 10, 10),
    .funcs_arena = arena_alloc(10 * sizeof(Func)),
    .minor_specs_arena = arena_alloc(10 * sizeof(Spec)),
    .func_arg_type_by_name_hds_arena = arena_alloc(10 * sizeof(HashDict))
  };

  ID *sum_func_id = func_id_alloc(&math_mod, "+", 1, 2, INT);
  func_id_add_arg(sum_func_id, "fst", 3, &int_type);
  func_id_add_arg(sum_func_id, "snd", 3, &int_type);
  sum_func_id->val.func->native_impl = sum_func_native_impl;

  Int n10 = 10, n25 = 25;
  HashDict *sum_func_eval_arg_by_name = hashdict_alloc(NULL, 2, 2);
  hashdict_add_entry(sum_func_eval_arg_by_name, "fst", 3, (char *)&n10, sizeof(Int));
  hashdict_add_entry(sum_func_eval_arg_by_name, "snd", 3, (char *)&n25, sizeof(Int));

  Value flowing_val;
  Value *res_val = func_eval(sum_func_id->val.func, sum_func_eval_arg_by_name, &flowing_val);
  printf("+(10, 25) => %lld\n", res_val->_int);

  hashdict_free(sum_func_eval_arg_by_name);
  func_id_free(sum_func_id);

  hashdict_free(math_mod.id_by_name);
  arena_free(math_mod.funcs_arena);
  arena_free(math_mod.ids_arena);
  arena_free(math_mod.minor_specs_arena);
  arena_free(math_mod.func_arg_type_by_name_hds_arena);

  // sum (fst: Int, snd: Int) -> Int \> +(fst, snd)

  // func args
  // HashDict *sum_func_arg_type_by_name = hashdict_alloc(
  //   math_mod.func_arg_type_by_name_hds_arena,
  //   2, 2
  // );
  // hashdict_add_entry(
  //   sum_func_arg_type_by_name,
  //   "fst",
  //   3,
  //   (char *)&int_type,
  //   sizeof(Type)
  // );
  // hashdict_add_entry(
  //   sum_func_arg_type_by_name,
  //   "snd",
  //   3,
  //   (char *)&int_type,
  //   sizeof(Type)
  // );
  // 
  // // func id
  // ID *sum_func_id = arena_malloc(math_mod.ids_arena, sizeof(ID));
  // sum_func_id->spec.type = FUNC;
  // sum_func_id->is_anon = false;
  // sum_func_id->name = "sum";

  // // func func
  // sum_func_id->value.func = arena_malloc(math_mod.funcs_arena, sizeof(Func));
  // sum_func_id->value.func->id = sum_func_id;
  // sum_func_id->value.func->val = NULL;
  // sum_func_id->value.func->native_impl = sum_func_native_impl;

  // // func spec
  // sum_func_id->minor_specs = arena_malloc(
  //   math_mod.minor_specs_arena,
  //   2 * sizeof(Spec)
  // );
  // // arg types
  // sum_func_id->minor_specs[0].type_by_name = sum_func_arg_type_by_name;
  // // return type
  // sum_func_id->minor_specs[1].type = INT;

  // hashdict_add_entry(
  //   math_mod.id_by_name,
  //   sum_func_id->name, 3,
  //   (char *)sum_func_id, sizeof(ID)
  // );

  // printf("sum (fst: Int, snd: Int) :: Int \\> +(fst, snd)\n");

  // HashDict *sum_call_arg_by_name = hashdict_alloc(NULL, 2, 2);
  // char n10 = (char)(Int)10;
  // hashdict_add_entry(
  //   sum_call_arg_by_name,
  //   "fst", 3,
  //   &n10, sizeof(Int)
  // );
  // hashdict_add_entry(
  //   sum_call_arg_by_name,
  //   "snd", 3,
  //   &n10, sizeof(Int)
  // );
  // Value flowing_val = { ._int = 11 };
  // Value *val = sum_func_native_impl(sum_call_arg_by_name, &flowing_val);
  // printf("sum(10, 10) => %lld\n", val->_int);

  // hashdict_free(sum_func_arg_type_by_name);

  // Flow flow;
  // flow.mods_arena = arena_alloc(sizeof(Module));

  // Module *core_mod = arena_malloc(flow.mods_arena, sizeof(Module));
  // core_mod->ids_arena = arena_alloc(128 * sizeof(ID));
  // core_mod->func_specs_arena = arena_alloc(56 * sizeof(HashDict));
  // core_mod->minor_specs_arena = arena_alloc(56 * sizeof(Spec));
  // core_mod->id_by_name = hashdict_alloc(NULL, 128, 256);

  // flow.cur_mod = core_mod;

  // // Str
  // ID *name = arena_malloc(core_mod->ids_arena, sizeof(ID));
  // name->spec.type = STR;
  // name->name = "name";
  // name->value.str = "João da Silva";

  // HashDictEntry *name_entry = hashdict_add_entry(
  //   core_mod.id_by_name,
  //   (char *)name->name, strlen(name->name),
  //   (char *)&name->value, sizeof(Value)
  // );
  // printf("name () :: Str \\> %s\n", name->value.str);

  // Int
  // ID *age = arena_malloc(core_mod.ids_arena, sizeof(ID));
  // age->spec.type = INT;
  // age->name = "age";
  // age->value._int = 18;
  //  
  // HashDictEntry *age_entry = hashdict_add_entry(
  //   core_mod.id_by_name,
  //   (char *)age->name, strlen(age->name),
  //   (char *)&age->value, sizeof(Value)
  // );
  // printf("age () :: Int \\> %lld\n", age->value._int);
  //  
  // // Float
  // ID *height = arena_malloc(core_mod.ids_arena, sizeof(ID));
  // height->spec.type = FLOAT;
  // height->name = "height";
  // height->value._float = 1.78f;
  //  
  // HashDictEntry *height_entry = hashdict_add_entry(
  //   core_mod.id_by_name,
  //   (char *)height->name, strlen(height->name),
  //   (char *)&height->value, sizeof(Value)
  // );
  // printf("height () :: Float \\> %.2fm\n", height->value._float);
  //  
  // // Bool
  // ID *can_drive = arena_malloc(core_mod.ids_arena, sizeof(ID));
  // can_drive->spec.type = BOOL;
  // can_drive->name = "can-drive?";
  // can_drive->value._bool = true;
  //  
  // HashDictEntry *can_drive_entry = hashdict_add_entry(
  //   core_mod.id_by_name,
  //   (char *)can_drive->name, strlen(height->name),
  //   (char *)&can_drive->value, sizeof(Value)
  // );
  // printf("can-drive? () :: Bool \\> %d\n", (int)can_drive->value._bool);
  //  
  // // Keyword
  // ID *my_keyword = arena_malloc(core_mod.ids_arena, sizeof(ID));
  // my_keyword->spec.type = KEYWORD;
  // my_keyword->name = "my-keyword";
  // my_keyword->value.keyword = "keyword";
  // printf("my-keyword () :: Keyword \\> :%s\n", my_keyword->value.keyword);
  //  
  // // List
  // ID *heights = arena_malloc(core_mod.ids_arena, sizeof(ID));
  // heights->spec.type = LIST;
  // heights->minor_specs = arena_malloc(
  //   core_mod.minor_specs_arena,
  //   sizeof(Spec)
  // );
  // heights->minor_specs[0].type = FLOAT;
  // heights->value.list.id = heights;

  // heights->value.list.vals_arena = arena_alloc(2 * sizeof(Float));
  // Arena *heights_arena = heights->value.list.vals_arena;
  // Float fst_height = arena_malloc(heights_arena, sizeof(Float));
  // Float snd_height = arena_malloc(heights_arena, sizeof(Float));

  // Func
  // ID *add = arena_malloc(core_mod.ids_arena, sizeof(ID));
  // add->spec.type_by_name = hashdict_alloc(core_mod.);

  // hashdict_free(core_mod.id_by_name);
  // arena_free(core_mod.ids_arena);
  // arena_free(core_mod.func_specs_arena);

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
    murmurhash3(hello, strlen(hello)),
    murmurhash3(bye, strlen(bye))
  );
 
  */

  return 0;
}
