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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./arena.h"
#include "./flow.h"
#include "./func.h"
#include "./hashdict.h"
#include "./id.h"
#include "./types.h"

void
sum_func_native_impl(HashDict *arg_by_name, Value *flowing_val)
{
    assert(arg_by_name);
    assert(flowing_val);

    HashDictEntry *fst_entry = hashdict_get_entry(arg_by_name, "fst", 3);
    HashDictEntry *snd_entry = hashdict_get_entry(arg_by_name, "snd", 3);

    Int fst = ((Value *)fst_entry->val)->_int;
    Int snd = ((Value *)snd_entry->val)->_int;
    (*flowing_val)._int = fst + snd;
}

int
main(void)
{
    // INFO.
    log_set_level(0);

    Module math_mod = { .ids_arena = arena_alloc(10 * sizeof(ID)),
                        .id_by_name = hashdict_alloc(NULL, NULL, 10, 10),
                        .funcs_arena = arena_alloc(10 * sizeof(Func)),
                        .minor_specs_arena = arena_alloc(10 * sizeof(Spec)),
                        .func_arg_type_by_name_hds_arena =
                          arena_alloc(10 * sizeof(HashDict)) };

    // + (fst Int, snd Int) :: Int => +(fst, snd)
    ID *sum_func_id =
      func_id_alloc(&math_mod, "+", 1, sum_func_native_impl, 2, INT, FUNC);
    func_id_add_arg(sum_func_id, "fst", 3, INT);
    func_id_add_arg(sum_func_id, "snd", 3, INT);

    Flow core_flow = { .frames_arena = arena_alloc(2 * sizeof(FlowFrame)),
                       .child_frame_values_arena =
                         arena_alloc(20 * sizeof(Value)) };

    // The following 3 frames constructs the expression:
    // +(+(10, 55), +(20, 25))

    // +(10, 55)
    FlowFrame *left_frame =
      arena_malloc(core_flow.frames_arena, sizeof(FlowFrame));
    left_frame->func_id = sum_func_id;
    left_frame->arg_by_name = hashdict_alloc(NULL, NULL, 5, 5);
    left_frame->frame_by_arg_name = NULL;

    Value left_fst_val = { ._int = 10 };
    Value left_snd_val = { ._int = 55 };
    hashdict_add_entry(
      left_frame->arg_by_name, "fst", 3, (char *)&left_fst_val, sizeof(Value));
    hashdict_add_entry(
      left_frame->arg_by_name, "snd", 3, (char *)&left_snd_val, sizeof(Value));

    core_flow.top_frame = left_frame;

    // +(20, 25)
    FlowFrame *right_frame =
      arena_malloc(core_flow.frames_arena, sizeof(FlowFrame));
    right_frame->func_id = sum_func_id;
    right_frame->arg_by_name = hashdict_alloc(NULL, NULL, 5, 5);
    right_frame->frame_by_arg_name = NULL;

    left_frame->next = right_frame;

    Value right_fst_val = { ._int = 20 };
    Value right_snd_val = { ._int = 25 };
    hashdict_add_entry(right_frame->arg_by_name,
                       "fst",
                       3,
                       (char *)&right_fst_val,
                       sizeof(Value));
    hashdict_add_entry(right_frame->arg_by_name,
                       "snd",
                       3,
                       (char *)&right_snd_val,
                       sizeof(Value));

    // +(let_frame, right_frame)
    FlowFrame *parent_frame =
      arena_malloc(core_flow.frames_arena, sizeof(FlowFrame));
    parent_frame->func_id = sum_func_id;
    parent_frame->next = NULL;
    parent_frame->arg_by_name = hashdict_alloc(NULL, NULL, 5, 5);
    parent_frame->frame_by_arg_name = hashdict_alloc(NULL, NULL, 5, 5);

    right_frame->parent = parent_frame;

    hashdict_add_entry(parent_frame->frame_by_arg_name,
                       "fst",
                       3,
                       (char *)left_frame,
                       sizeof(FlowFrame));
    hashdict_add_entry(parent_frame->frame_by_arg_name,
                       "snd",
                       3,
                       (char *)right_frame,
                       sizeof(FlowFrame));

    Value *res = flow_eval(&core_flow);
    printf("+(+(10, 55), +(20, 25)) => %lld\n", res->_int); // 110.

    hashdict_free(left_frame->arg_by_name);
    hashdict_free(right_frame->arg_by_name);
    hashdict_free(parent_frame->arg_by_name);
    hashdict_free(parent_frame->frame_by_arg_name);
    flow_free(&core_flow);
    id_free(sum_func_id);

    hashdict_free(math_mod.id_by_name);
    arena_free(math_mod.funcs_arena);
    arena_free(math_mod.ids_arena);
    arena_free(math_mod.minor_specs_arena);
    arena_free(math_mod.func_arg_type_by_name_hds_arena);

    return 0;
}
