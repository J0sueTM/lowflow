#include "./runtime.h"

static void _lf_reverse_val_schedule(
  LF_Flow *flow,
  LF_Stack *reverse_val_schedule
) {
  lf_log_debug(
    &flow->logger,
    "beg reverse val schedule [flow=%x]",
    flow
  );
  
  flow->val_schedule.elem_size = sizeof(LF_Value *);
  flow->val_schedule.elem_alignment = alignof(LF_Value *);
  flow->val_schedule.elem_qtt_in_block = LF_FLOW_SCHEDULE_VAL_QTT_IN_BLOCK;
  lf_init_list(&flow->val_schedule);

  LF_Value **cur_val;
  while (!lf_is_stack_empty(reverse_val_schedule)) {
    cur_val = (LF_Value **)lf_pop_from_stack(
      reverse_val_schedule
    );
    LF_Value **scheduled_val = (LF_Value **)lf_alloc_list_elem(
      &flow->val_schedule, 1
    );
    *scheduled_val = *cur_val;
  }

  lf_log_debug(
    &flow->logger,
    "end reverse val schedule [flow=%x]",
    flow
  );
}

// NOTE: This function assumes that when encountering a func call,
// function calls come before primitives. This ensures correct depth  
// first search visit path.
static void _lf_build_val_schedule(
  LF_Flow *flow,
  LF_Value *entrypoint
) {
  assert(flow);
  assert(entrypoint);

  lf_log_debug(&flow->logger, "beg build flow val schedule [flow=%x]", flow);

#ifndef LF_FLOW_VAL_TO_VISIT_QTT_IN_BLOCK
#define LF_FLOW_VAL_TO_VISIT_QTT_IN_BLOCK 256
#endif

#ifndef LF_FLOW_VISITED_VAL_QTT_IN_BLOCK
#define LF_FLOW_VAL_TO_VISIT_QTT_IN_BLOCK 256
#endif

  LF_Stack vals_to_visit = {
    .elem_size = sizeof(LF_Value *),
    .elem_alignment = alignof(LF_Value *),
    .elem_qtt_in_block = LF_FLOW_VAL_TO_VISIT_QTT_IN_BLOCK
  };
  lf_init_stack(&vals_to_visit);
  LF_Value **entrypoint_val = (LF_Value **)lf_alloc_stack_elem(
    &vals_to_visit
  );
  *entrypoint_val = entrypoint;

  LF_Stack reverse_val_schedule = {
    .elem_size = sizeof(LF_Value *),
    .elem_alignment = alignof(LF_Value *),
    .elem_qtt_in_block = LF_FLOW_SCHEDULE_VAL_QTT_IN_BLOCK
  };
  lf_init_stack(&reverse_val_schedule);

  while (!lf_is_stack_empty(&vals_to_visit)) {
    LF_Value **tmp_cur_val = (LF_Value **)lf_pop_from_stack(&vals_to_visit);
    // NOTE: Future (*tmp_cur_val) wouldn't work because it will be
    // overwritten at the next stack push. That's why we get the
    // pointer back into another variable, so we don't lose it.
    LF_Value *cur_val = *tmp_cur_val;

    bool already_scheduled = (bool)lf_get_stack_elem_by_content(
      &reverse_val_schedule,
      (char *)&cur_val
    );
    if (already_scheduled) {
      continue;
    }

    LF_Value **rev_scheduled_val = (LF_Value **)lf_alloc_stack_elem(
      &reverse_val_schedule
    );
    *rev_scheduled_val = cur_val;

    bool is_leaf = (cur_val->type & PRIMITIVE_MASK) != 0;
    if (is_leaf) {
      continue;
    }

    // If not a leaf (primitive), it's a function.
    size_t arg_qtt = cur_val->inner_val->func_def_spec->arg_qtt;
    for (size_t i = 0; i < arg_qtt; ++i) {
      LF_Value **arg_val_to_visit = (LF_Value **)lf_alloc_stack_elem(
        &vals_to_visit
      );
      *arg_val_to_visit = &cur_val->func_call_spec->args[i];
    }
  }

  _lf_reverse_val_schedule(flow, &reverse_val_schedule);
  
  lf_log_debug(
    &flow->logger,
    "end init flow [flow=%x, scheduled_count=%d]",
    flow,
    flow->val_schedule.elem_count
  );
}

void lf_init_flow(LF_Flow *flow, LF_Value *entrypoint) {
  assert(flow);
  assert(entrypoint);

  flow->logger.min_level = LF_DEBUG;
  flow->logger.time_fmt = NULL;
  lf_init_logger(&flow->logger);

  lf_log_debug(&flow->logger, "beg init flow [flow=%x]", flow);

  _lf_build_val_schedule(flow, entrypoint);

  flow->frame_vals.elem_qtt_in_block = (
    LF_FLOW_FRAME_VAL_QTT_IN_BLOCK * sizeof(LF_Value *)
  );
  flow->frame_vals.elem_size = sizeof(LF_Value *);
  flow->frame_vals.elem_alignment = alignof(LF_Value *);
  lf_init_stack(&flow->frame_vals);

  flow->new_vals.elem_qtt_in_block = (
    LF_FLOW_NEW_VAL_QTT_IN_BLOCK * sizeof(LF_Value)
  );
  flow->new_vals.elem_size = sizeof(LF_Value);
  flow->new_vals.elem_alignment = alignof(LF_Value);
  lf_init_stack(&flow->new_vals);

  lf_log_debug(&flow->logger, "end init flow [flow=%x]", flow);
}

void lf_eval_flow(LF_Flow *flow) {
  assert(flow);

  lf_log_debug(&flow->logger, "beg eval flow [flow=%x]", flow);

  LF_Value **cur_val = (LF_Value **)lf_get_first_list_elem(&flow->val_schedule);
  while (cur_val) {
    switch ((*cur_val)->type) {
      case LF_INT:
      case LF_FLOAT:
      case LF_CHAR:
      case LF_STR:
      case LF_BOOL: {
        LF_Value **frame_val = (LF_Value **)lf_alloc_stack_elem(&flow->frame_vals);
        *frame_val = *cur_val;
      } break;
      case LF_FUNC_CALL: {
        // TODO: Maybe reuse old vals?
        LF_Value *new_res_val = (LF_Value *)lf_alloc_stack_elem(&flow->new_vals);
        LF_Value *func_def = (*cur_val)->inner_val;
        if (!func_def->func_def_spec->native_impl) {
          lf_log_fatal(
            &flow->logger,
            "eval flow: func has no native impl [flow=%x, def=%x]",
            flow,
            func_def
          );
        }

        size_t func_arg_qtt = func_def->func_def_spec->arg_qtt;
        size_t frame_val_qtt = flow->frame_vals.elem_count;
        if (frame_val_qtt < func_arg_qtt) {
          lf_log_fatal(
            &flow->logger,
            "eval flow: not enough vals in frame for func call [flow=%x, expected=%d, got=%d]",
            flow,
            func_arg_qtt,
            frame_val_qtt
          );
          // TODO: Stuff above the flow expects results. This should
          // return an Option instead of stop everything.
          goto finish_eval;
        }
        
        func_def->func_def_spec->native_impl(new_res_val, &flow->frame_vals);
        lf_reset_stack(&flow->frame_vals);
        LF_Value **res_in_frame = (LF_Value **)lf_alloc_stack_elem(
          &flow->frame_vals
        );
        *res_in_frame = new_res_val;
      } break;
      case LF_SYMBOL:
      case LF_LIST:
      case LF_SEQ:
      case LF_DICT:
      default:
        assert(false && "not implemented");
        break;
    }
    cur_val = (LF_Value **)lf_get_next_list_elem(
      &flow->val_schedule,
      (char *)cur_val
    );
  }

finish_eval:
  lf_log_debug(&flow->logger, "end eval flow [flow=%x]", flow);
}
