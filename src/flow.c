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

#include "./flow.h"
#include "arena.h"
#include "hashdict.h"

void
flow_free(Flow *flow)
{
    assert(flow);
    arena_free(flow->frames_arena);
}

void
add_frame_args_from_bindings(Flow *flow, FlowFrame *frame)
{
    assert(flow);
    assert(flow->child_frame_values_arena);
    assert(frame);
    assert(frame->frame_by_arg_name);
    assert(frame->arg_by_name);

    HashDictEntry *cur_head_entry = NULL;
    HashDictEntry *cur_entry = NULL;
    for (size_t i = 0; i < frame->frame_by_arg_name->head_entry_cap; ++i) {
        cur_head_entry = frame->frame_by_arg_name->heads + i;
        if (!cur_head_entry->key) {
            continue;
        }

        cur_entry = cur_head_entry;
        while (cur_entry) {
            if (!cur_entry->key && cur_entry->val) {
                cur_entry = cur_entry->next;
                continue;
            }

            FlowFrame *arg_frame = (FlowFrame *)cur_entry->val;
            Value *binded_val = arena_malloc(flow->child_frame_values_arena, sizeof(Value));

            // TODO: This works for types like Int or Bool, but not for complex datatypes.
            // Fix me later.
            *binded_val = arg_frame->val;

            // TODO: Copy keys correctly.
            hashdict_add_entry(frame->arg_by_name,
                               cur_entry->key,
                               cur_entry->key_size,
                               (char *)binded_val,
                               sizeof(Value));

            cur_entry = cur_entry->next;
        }
    }
}

void
flow_frame_eval(Flow *flow, FlowFrame *frame)
{
    assert(flow);
    assert(frame);

    ID *func_id = frame->func_id;
    assert(func_id);
    log_trace("beg eval frame(%p) [func_id=%p]", frame, func_id);

    if (frame->frame_by_arg_name) {
        add_frame_args_from_bindings(flow, frame);
    }
    hashdict_debug(frame->arg_by_name);

    Func *func = func_id->val.func;
    if (func->native_impl) {
        func->native_impl(frame->arg_by_name, &frame->val);
        log_debug(
          "end eval frame(%p) [native_impl=%p]", frame, func->native_impl);
        return;
    }

    // TODO: clean old structure if needed. Maybe it should be done before
    // caling the native implementation as well.
    assert(func_id->minor_specs);
    assert(func->val);
    Type ret_type = func_id->minor_specs[1].type;
    switch (ret_type) {
        case INT:
            flow->val._int = func->val->_int;
            break;
        case FLOAT:
            flow->val._float = func->val->_float;
            break;
        case BOOL:
            flow->val._bool = func->val->_bool;
            break;
        case STR:
        case KEYWORD:
        case LIST:
        case DICT:
        case SEQ:
        case STRUCT:
        case FUNC:
        case TYPE:
        default:
            log_error("invalid ret type %d [frame=%p, func_id=%p]",
                      ret_type,
                      frame,
                      func_id);
            break;
    }

    log_debug("end eval frame(%p) [native_impl=(nil)]", frame);
}

Value *
flow_eval(Flow *flow)
{
    assert(flow);
    assert(flow->frames_arena);
    assert(flow->child_frame_values_arena);
    assert(flow->top_frame);

    log_trace("beg eval flow(%p)", flow);

    arena_reset(flow->child_frame_values_arena);

    FlowFrame *cur_frame = flow->top_frame;
    FlowFrame *last_frame = cur_frame;
    bool finished_child_frames = false;
    while (cur_frame) {
        flow_frame_eval(flow, cur_frame);
        if (finished_child_frames) {
            arena_reset(flow->child_frame_values_arena);
        }

        last_frame = cur_frame;
        finished_child_frames = cur_frame->parent;
        cur_frame = (cur_frame->parent) ? cur_frame->parent : cur_frame->next;
    }

    log_debug("end eval flow(%p)", flow);

    return &last_frame->val;
}
