#include "./flow_partition.h"

void lf_partition_flows(LF_PassPipeline *pipeline) {
  assert(pipeline);
  assert(&pipeline->val_schedule);

  lf_log_debug(&pipeline->logger,
               "build_flow_schedule: begin. pipeline=%p ",
               pipeline);

  LF_List flows = {
    .elem_size = sizeof(LF_Flow),
    .elem_alignment = alignof(LF_Flow),
    .elem_qtt_in_block = LF_PASS_FLOW_PARTITION_FLOW_QTT_IN_BLOCK};
  lf_init_list(&flows);

  LF_Flow *parent_flow = (LF_Flow *)lf_alloc_list_elem(&flows);
  parent_flow->logger.min_level = LF_FLOW_LOGGER_MIN_LEVEL;
  parent_flow->logger.time_fmt = NULL;
  lf_init_logger(&parent_flow->logger);

  if (pipeline->flow_partition_strategy == LF_FLOW_PARTITION_STRATEGY_NONE) {
    // lf_copy_stack(&pipeline->val_schedule, &parent_flow->val_schedule);
    // parent_flow->val_schedule = pipeline->val_schedule;
  }

  lf_log_debug(&pipeline->logger,
               "build_val_schedule: end. pipeline=%p ",
               pipeline);
}
