#include "./val_schedule.h"

void lf_reverse_val_schedule(
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
void lf_build_val_schedule(
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

  lf_reverse_val_schedule(flow, &reverse_val_schedule);
  
  lf_log_debug(
    &flow->logger,
    "end init flow [flow=%x, scheduled_count=%d]",
    flow,
    flow->val_schedule.elem_count
  );
}

void lf_build_val_schedule_new(
  LF_PassContext *ctx
) {
  assert(ctx->entrypoint);

  lf_log_debug(
    &ctx->logger,
    "beg build val schedule [vs=%x, e=%x]",
    ctx->val_schedule,
    ctx->entrypoint
  );

  LF_Stack vals_to_visit = {
    .elem_size = sizeof(LF_Value *),
    .elem_alignment = alignof(LF_Value *),
    .elem_qtt_in_block = ctx->val_schedule.elem_qtt_in_block
  };
  lf_init_stack(&vals_to_visit);
  LF_Value **entrypoint_val = (LF_Value **)lf_alloc_stack_elem(
    &vals_to_visit
  );
  *entrypoint_val = ctx->entrypoint;

  while (!lf_is_stack_empty(&vals_to_visit)) {
    LF_Value **tmp_cur_val = (LF_Value **)lf_pop_from_stack(&vals_to_visit);
    // NOTE: Future (*tmp_cur_val) wouldn't work because it will be
    // overwritten at the next stack push. That's why we get the
    // pointer back into another variable, so we don't lose it.
    LF_Value *cur_val = *tmp_cur_val;

    bool already_scheduled = (bool)lf_get_stack_elem_by_content(
      &ctx->val_schedule,
      (char *)&cur_val
    );
    if (already_scheduled) {
      continue;
    }

    LF_Value **scheduled_val = (LF_Value **)lf_alloc_stack_elem(
      &ctx->val_schedule
    );
    *scheduled_val = cur_val;

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

  lf_log_debug(
    &ctx->logger,
    "end build val schedule [vs=%x]",
    ctx->val_schedule
  );
}
