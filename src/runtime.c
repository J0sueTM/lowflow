#include "./runtime.h"

void lf_init_flow(LF_Flow *flow, LF_Value *entrypoint) {
  assert(flow);
  assert(entrypoint);

  flow->logger.min_level = LF_DEBUG;
  flow->logger.time_fmt = NULL;
  lf_init_logger(&flow->logger);

  lf_log_debug(&flow->logger, "beg init flow [flow=%x]", flow);

#ifndef LF_FLOW_VISITED_VAL_QTT_IN_BLOCK
#define LF_FLOW_VISITED_VAL_QTT_IN_BLOCK 500
#endif

  LF_List visited_vals = {
    .elem_size = sizeof(LF_Value *),
    .elem_alignment = alignof(LF_Value *),
    .elem_qtt_in_block = LF_FLOW_VISITED_VAL_QTT_IN_BLOCK
  };
  lf_init_list(&visited_vals);

  LF_Value **entrypoint_val = (LF_Value **)lf_alloc_list_elem(
      &visited_vals, 1
  );
  *entrypoint_val = entrypoint;

  LF_Value **cur_val = entrypoint_val;
  do {
    bool is_leaf = ((*cur_val)->type & PRIMITIVE_MASK) != 0;
    if (is_leaf) {
      goto next_visited_val;
    }

    size_t arg_qtt = (*cur_val)->inner_val->func_def_spec->arg_qtt;
    LF_Value **arg_vals = (LF_Value **)lf_alloc_list_elem(
      &visited_vals, arg_qtt
    );
    for (size_t i = 0; i < arg_qtt; ++i) {
      arg_vals[i] = &(*cur_val)->func_call_spec->args[i];
    }

  next_visited_val:
    cur_val = (LF_Value **)lf_get_next_list_elem(
      &visited_vals,
      (char *)cur_val
    );
  } while (cur_val);

  flow->val_schedule.elem_size = sizeof(LF_Value *);
  flow->val_schedule.elem_alignment = alignof(LF_Value *);
  flow->val_schedule.elem_qtt_in_block = LF_FLOW_FRAME_VAL_QTT_IN_BLOCK;
  lf_init_list(&flow->val_schedule);

  LF_Value **cur_visited_val = (LF_Value **)lf_get_last_list_elem(
    &visited_vals
  );
  while (cur_visited_val) {
    LF_Value **val_to_schedule = (LF_Value **)lf_alloc_list_elem(
      &flow->val_schedule, 1
    );
    *val_to_schedule = *cur_visited_val;

    cur_visited_val = (LF_Value **)lf_get_prev_list_elem(
      &visited_vals,
      (char *)cur_visited_val
    );
  }

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

  lf_log_debug(
    &flow->logger,
    "end init flow [flow=%x, elem_count=%ld]",
    flow,
    flow->val_schedule.elem_count
  );
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
            "eval flow: func has no native impl [f=%x, fu=%x]",
            flow,
            func_def
          );
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

  lf_log_debug(&flow->logger, "end eval flow [flow=%x]", flow);
}
