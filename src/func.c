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

#include "./types.h"

ID *func_id_alloc(
  Module *mod,
  char *name,
  size_t name_size,
  size_t arg_count,
  Type return_type
) {
  assert(mod);
  assert(mod->ids_arena);
  assert(mod->id_by_name);
  assert(mod->minor_specs_arena);
  assert(mod->funcs_arena);
  assert(mod->func_arg_type_by_name_hds_arena);

  ID *id = arena_malloc(mod->ids_arena, sizeof(ID));
  assert(id);

  id->spec.type = FUNC;
  id->is_anon = false;
  id->name = name;

  HashDict *arg_type_by_name = hashdict_alloc(
    mod->func_arg_type_by_name_hds_arena,
    arg_count, arg_count
  );
  assert(arg_type_by_name);

  id->minor_specs = arena_malloc(
    mod->minor_specs_arena,
    // 0: arg types, 1: return type
    2 * sizeof(Spec)
  );
  id->minor_specs[0].type_by_name = arg_type_by_name;
  id->minor_specs[1].type = return_type;

  id->val.func = arena_malloc(mod->funcs_arena, sizeof(Func));
  id->val.func->id = id;

  hashdict_add_entry(
    mod->id_by_name,
    name,
    name_size,
    (char *)id,
    sizeof(ID)
  );
}

HashDictEntry *func_id_add_arg(
  ID *func_id,
  char *name,
  size_t name_size,
  Type *type
) {
  assert(func_id);
  assert(name);

  return hashdict_add_entry(
    func_id->minor_specs[0].type_by_name,
    name, name_size,
    (char *)type, sizeof(Type)
  );
}

void func_id_free(ID *func_id) {
  assert(func_id);
  assert(func_id->minor_specs[0].type_by_name);

  hashdict_free(func_id->minor_specs[0].type_by_name);
}

Value *func_eval(
  Func *func,
  HashDict *arg_by_name,
  Value *flowing_val
) {
  assert(func);
  assert(arg_by_name);
  assert(func->native_impl);

  return func->native_impl(arg_by_name, flowing_val);
}
