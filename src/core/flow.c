#include "./flow.h"

void lf_init_flow(LF_Flow *flow) {
  assert(flow);
  assert(flow->val_schedule.elem_qtt_in_block >= 2);

  flow->logger.min_level = LF_FLOW_LOGGER_MIN_LEVEL;
  flow->logger.time_fmt = NULL;
  lf_init_logger(&flow->logger);

  // flow->val_schedule.elem_qtt_in_block needs to be defined before flow initialization.
  flow->val_schedule.elem_size = sizeof(LF_Value *);
  flow->val_schedule.elem_alignment = alignof(LF_Value *);

  // TODO: Look into how memory is spreaded, in the future.
  // There's no gain in using arenas if everything allocates
  // stuff everywhere.
  lf_init_list(&flow->val_schedule);

  flow->frame_vals.elem_size = sizeof(LF_Value *);
  flow->frame_vals.elem_alignment = alignof(LF_Value *);
  flow->frame_vals.elem_qtt_in_block = LF_FLOW_FRAME_VAL_QTT_IN_BLOCK;
  lf_init_stack(&flow->frame_vals);

  flow->new_vals.elem_size = sizeof(LF_Value);
  flow->new_vals.elem_alignment = alignof(LF_Value);
  flow->new_vals.elem_qtt_in_block = LF_FLOW_NEW_VAL_QTT_IN_BLOCK;
  lf_init_stack(&flow->new_vals);
}
