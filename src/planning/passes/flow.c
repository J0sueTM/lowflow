#include "./flow.h"

void lf_init_flow(LF_Flow *flow, LF_Value *entrypoint) {
  assert(flow);
  assert(entrypoint);

  flow->logger.min_level = LF_DEBUG;
  flow->logger.time_fmt = NULL;
  lf_init_logger(&flow->logger);

  lf_log_debug(&flow->logger, "beg init flow [flow=%x]", flow);

  // lf_build_val_schedule(flow, entrypoint);

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

