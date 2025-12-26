#include "./val_schedule.h"

void lf_build_val_schedule(LF_PassPipeline *pipeline) {
  assert(pipeline);
  assert(pipeline->entrypoint);

  lf_log_debug(&pipeline->logger,
               "build_val_schedule: begin. pipeline=%p "
               "entrypoint=%p",
               pipeline,
               pipeline->entrypoint);

  LF_Stack vals_to_visit = {
    .elem_size = sizeof(LF_Value *),
    .elem_alignment = alignof(LF_Value *),
    .elem_qtt_in_block = pipeline->val_schedule.elem_qtt_in_block,
  };
  lf_init_stack(&vals_to_visit);

  LF_Value **entrypoint_val = (LF_Value **)lf_alloc_stack_elem(&vals_to_visit);
  *entrypoint_val = pipeline->entrypoint;

  while (!lf_is_stack_empty(&vals_to_visit)) {
    LF_Value *cur_val = *(LF_Value **)lf_pop_from_stack(&vals_to_visit);

    bool already_scheduled =
      (bool)lf_get_stack_elem_by_content(&pipeline->val_schedule,
                                         (char *)&cur_val);
    if (already_scheduled) {
      continue;
    }

    LF_Value **scheduled_val =
      (LF_Value **)lf_alloc_stack_elem(&pipeline->val_schedule);
    *scheduled_val = cur_val;

    bool is_leaf = (cur_val->type & PRIMITIVE_MASK) != 0;
    if (is_leaf) {
      continue;
    }

    // If not a leaf (primitive), it's a function.
    size_t arg_qtt = cur_val->inner_val->func_def_spec->arg_qtt;
    for (size_t i = 0; i < arg_qtt; ++i) {
      LF_Value **arg_val_to_visit =
        (LF_Value **)lf_alloc_stack_elem(&vals_to_visit);
      *arg_val_to_visit = cur_val->func_call_spec->args[i];
    }
  }

  lf_log_debug(&pipeline->logger,
               "build_val_schedule: end. pipeline=%p "
               "val_schedule=%p "
               "val_count=%zu",
               pipeline,
               &pipeline->val_schedule,
               pipeline->val_schedule.elem_count);
}
