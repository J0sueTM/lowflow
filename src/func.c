/*
 * Copyright (C) Josué Teodoro Moreira
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <https://www.gnu.org/licenses/>.
 */

#include "./func.h"

ID *
func_id_alloc(Module *mod,
              char *name,
              size_t name_size,
              void (*native_impl)(HashDict *arg_by_name, Value *flowing_val),
              size_t arg_count,
              Type return_type,
              Type val_type)
{
    assert(mod);
    assert(mod->ids_arena);
    assert(mod->id_by_name);
    assert(mod->minor_specs_arena);
    assert(mod->funcs_arena);
    assert(mod->func_arg_type_by_name_hds_arena);

    log_trace("beg alloc func_id [name='%s', argc=%lld]", name, arg_count);

    ID *id = arena_malloc(mod->ids_arena, sizeof(ID));
    assert(id);

    id->spec.type = FUNC;
    id->is_anon = false;
    id->name = name;

    id->minor_specs = arena_malloc(mod->minor_specs_arena,
                                   // 0: arg types
                                   // 1: return type
                                   2 * sizeof(Spec));
    if (arg_count > 0) {
        // TODO: create outter arena for arg type entries.
        id->minor_specs[0].type_by_name = hashdict_alloc(
          mod->func_arg_type_by_name_hds_arena, NULL, arg_count, arg_count);
    }
    id->minor_specs[1].type = return_type;

    id->val.func = arena_malloc(mod->funcs_arena, sizeof(Func));
    id->val.func->id = id;
    id->val.func->native_impl = native_impl;

    hashdict_add_entry(
      mod->id_by_name, name, name_size, (char *)id, sizeof(ID));

    log_debug("end alloc func_id(%p) [name='%s']", id, name);

    return id;
}

// Each type enum points to themselves here. (int)STR = 0, for ex.
// This is used for places where an allocated type is needed, like
// when using a type as a hashdict value.
static Type allocated_types[TYPE_COUNT] = { STR,     INT,  FLOAT, BOOL,
                                            KEYWORD, LIST, DICT,  SEQ,
                                            STRUCT,  FUNC, TYPE };

HashDictEntry *
func_id_add_arg(ID *func_id, char *name, size_t name_size, Type type)
{
    assert(func_id);
    assert(name);

    log_trace("beg func_id_add_arg [func_id=%p, func_name=%s, arg_name=%s, "
              "arg_type=%d]",
              func_id,
              func_id->name,
              name,
              type);

    // Beautiful `gambiarra`.
    short allocated_type_idx = (short)type;
    assert(allocated_type_idx < TYPE_COUNT);

    HashDictEntry *arg_entry =
      hashdict_add_entry(func_id->minor_specs[0].type_by_name,
                         name,
                         name_size,
                         (char *)&allocated_types[allocated_type_idx],
                         sizeof(Type));

    log_debug("end func_id_add_arg entry(%p) [func_id=%p, arg_name=%s, "
              "arg_type=%d]",
              arg_entry,
              func_id,
              name,
              type);

    return arg_entry;
}

void
func_id_free(ID *func_id)
{
    assert(func_id);
    assert(func_id->minor_specs[0].type_by_name);

    hashdict_free(func_id->minor_specs[0].type_by_name);
}
