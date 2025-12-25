#include "./flow_partition.h"

void lf_partition_flows(LF_PassPipeline *pipeline) {
  assert(pipeline);
  assert(&pipeline->val_schedule);

  lf_log_debug(&pipeline->logger,
               "partition_flows: begin. pipeline=%p ",
               "strategy=%d",
               pipeline,
               pipeline->flow_partition_strategy);

  LF_List flows = {
    .elem_size = sizeof(LF_Flow),
    .elem_alignment = alignof(LF_Flow),
  };

  switch (pipeline->flow_partition_strategy) {
    case LF_FLOW_PARTITION_STRATEGY_NONE:
      flows.elem_qtt_in_block = 1;
      break;
    default:
      assert(false && "not implemented!");
      break;
  }

  lf_init_list(&flows);

  LF_Flow *parent_flow = (LF_Flow *)lf_alloc_list_elem(&flows);

  parent_flow->logger.min_level = LF_FLOW_LOGGER_MIN_LEVEL;
  parent_flow->logger.time_fmt = NULL;
  lf_init_logger(&parent_flow->logger);

  parent_flow->val_schedule.elem_size = sizeof(LF_Value *);
  parent_flow->val_schedule.elem_alignment = alignof(LF_Value *);
  switch (pipeline->flow_partition_strategy) {
    case LF_FLOW_PARTITION_STRATEGY_NONE:
      parent_flow->val_schedule.elem_qtt_in_block = pipeline->val_schedule.elem_qtt_in_block;
      break;
    default:
      assert(false && "not implemented!");
      break;
  }

  // TODO: Look into how memory is spreaded, in the future.
  // There's no gain in using arenas if everything allocates
  // stuffy everywhere.
  lf_init_list(&parent_flow->val_schedule);

  switch (pipeline->flow_partition_strategy) {
    case LF_FLOW_PARTITION_STRATEGY_NONE:
      lf_stack_to_list(&pipeline->val_schedule, &parent_flow->val_schedule);
      break;
    default:
      assert(false && "not implemented!");
      break;
  }

  pipeline->parent_flow = parent_flow;

  lf_log_debug(&pipeline->logger,
               "partition_flows: end. pipeline=%p flow_qtt=%lld "
               "parent_flow=%p",
               pipeline,
               flows.elem_count,
               parent_flow);
}
