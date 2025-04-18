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

Value *sum_func_native_impl(HashDict *arg_by_name,
                            Value *flowing_val) {
  HashDictEntry *fst_entry = hashdict_get_entry(arg_by_name,
                             "fst", 3);
  HashDictEntry *snd_entry = hashdict_get_entry(arg_by_name,
                             "snd", 3);

  Int fst = (Int)*fst_entry->val;
  Int snd = (Int)*snd_entry->val;

  flowing_val->_int = fst + snd;

  return flowing_val;
}

int main(void) {
  Module math_mod = {
    .ids_arena = arena_alloc(10 * sizeof(ID)),
    .id_by_name = hashdict_alloc(NULL, 10, 10),
    .funcs_arena = arena_alloc(10 * sizeof(Func)),
    .minor_specs_arena = arena_alloc(10 * sizeof(Spec)),
    .func_arg_type_by_name_hds_arena = arena_alloc(10 * sizeof(HashDict))
  };

  ID *sum_func_id = func_id_alloc(&math_mod, "+", 1, 2, INT);
  func_id_add_arg(sum_func_id, "fst", 3, INT);
  func_id_add_arg(sum_func_id, "snd", 3, INT);
  sum_func_id->val.func->native_impl = sum_func_native_impl;

  Int n10 = 10, n25 = 25;
  HashDict *sum_func_eval_arg_by_name = hashdict_alloc(NULL,
                                        2, 2);
  hashdict_add_entry(sum_func_eval_arg_by_name, "fst", 3,
                     (char *)&n10, sizeof(Int));
  hashdict_add_entry(sum_func_eval_arg_by_name, "snd", 3,
                     (char *)&n25, sizeof(Int));

  Value flowing_val;
  Value *res_val = func_eval(sum_func_id->val.func,
                             sum_func_eval_arg_by_name, &flowing_val);
  printf("+(10, 25) => %lld\n", res_val->_int);

  hashdict_free(sum_func_eval_arg_by_name);
  func_id_free(sum_func_id);

  hashdict_free(math_mod.id_by_name);
  arena_free(math_mod.funcs_arena);
  arena_free(math_mod.ids_arena);
  arena_free(math_mod.minor_specs_arena);
  arena_free(math_mod.func_arg_type_by_name_hds_arena);

  return 0;
}
