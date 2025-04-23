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

#include "./flow.h"

Flow *flow_build_tree(Arena *arena, ID *parent_id)
{
    assert(arena);
    assert(parent_id);

    Flow *flow = arena_malloc(arena, sizeof(Flow));
    flow->frames_arena = arena_alloc(
                             FLOW_FRAME_CAP * sizeof(FlowFrame)
                         );
    flow->arg_by_type_hds_arena = arena_alloc(FLOW_ARG_BY_TYPE_ARENA_CAP *
                                  sizeof(HashDict));
    flow->arg_entries_arena = arena_alloc(
                                  FLOW_ARG_ENTRY_CAP *
                                  sizeof(HashDictEntry)
                              );

    ID *cur_id = parent_id;
    ID *cur_parent_id = NULL;
    while (cur_id) {
        if (cur_id->spec.type != FUNC) {
            break;
        } else if (cur_id->val.func->native_impl) {
            break;
        }

        FlowFrame *cur_frame = arena_malloc(flow->frames_arena,
                                            sizeof(FlowFrame));
        cur_frame->parent_func_id = cur_parent_id;
        // TODO: Find an optimal size. I (jtm) think that this value
        // could be fixed after parsing real code, because we will
        // know beforehand the amount needed.
        cur_frame->arg_by_name = hashdict_alloc(flow->arg_by_type_hds_arena,
                                                flow->arg_entries_arena, 5, 10);

        // build arg_by_name
        HashDict *type_by_name = cur_id->minor_specs[1].type_by_name;
        size_t visited_head_count = 0;
        HashDictEntry *cur_head_entry = type_by_name->heads;
        HashDictEntry *cur_entry = cur_head_entry;
        for (int i = 0; i < type_by_name->entry_count; ++i) {
            assert(cur_entry->key);
            assert(cur_entry->key_size);

            hashdict_add_entry(cur_frame->arg_by_name, cur_entry->key,
                               cur_entry->key_size, NULL, 0);

            bool visited_all_heads = (visited_head_count <
                                       type_by_name->head_entry_cap);
            if (!cur_head_entry && !visited_all_heads) {
                ++visited_head_count;
                ++cur_head_entry;
                cur_entry = cur_head_entry;
                continue;
            } else if (!cur_head_entry && visited_all_heads) {
                break;
            }

            cur_entry = cur_entry->next;
            if (!cur_entry) {
                if (visited_all_heads) {
                    break;
                }

                ++visited_head_count;
                ++cur_head_entry;
            }
        }

        cur_parent_id = cur_id;
        cur_id = cur_id->val.func->child_id;
    }

    return flow;
}

void flow_free(Flow *flow) {
    assert(flow);

    // TODO URGENT: This will free keys from arg entries, causingp
    // douple free in the future
    arena_free(flow->arg_by_type_hds_arena);
    arena_free(flow->arg_entries_arena);
    arena_free(flow->frames_arena);
}
