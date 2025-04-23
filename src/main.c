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
#include "./id.h"
#include "./flow.h"

Value *sum_func_native_impl(HashDict *arg_by_name, Value *flowing_val)
{
    HashDictEntry *fst_entry = hashdict_get_entry(arg_by_name, "fst", 3);
    HashDictEntry *snd_entry = hashdict_get_entry(arg_by_name, "snd", 3);

    Int fst = ((Value*)fst_entry->val)->_int;
    Int snd = ((Value*)snd_entry->val)->_int;
    flowing_val->_int = fst + snd;

    return flowing_val;
}

int main(void)
{
    Module math_mod = {
        .ids_arena = arena_alloc(10 * sizeof(ID)),
        .id_by_name = hashdict_alloc(NULL, NULL, 10, 10),
        .funcs_arena = arena_alloc(10 * sizeof(Func)),
        .minor_specs_arena = arena_alloc(10 * sizeof(Spec)),
        .func_arg_type_by_name_hds_arena = arena_alloc(10 * sizeof(HashDict))
    };

    // + (fst Int, snd Int)
    ID *sum_func_id = func_id_alloc(&math_mod, "+", 1,
                                    sum_func_native_impl, 2, INT, FUNC);
    func_id_add_arg(sum_func_id, "fst", 3, INT);
    func_id_add_arg(sum_func_id, "snd", 3, INT);

    // This will probably go into a cluster later on.
    Arena *flows_arena = arena_alloc(1 * sizeof(Flow));
    
    Flow *flow = flow_build_tree(flows_arena, sum_func_id);

    // flow_free(flow)
    id_free(sum_func_id);

    hashdict_free(math_mod.id_by_name);
    arena_free(math_mod.funcs_arena);
    arena_free(math_mod.ids_arena);
    arena_free(math_mod.minor_specs_arena);
    arena_free(math_mod.func_arg_type_by_name_hds_arena);

    return 0;
}
