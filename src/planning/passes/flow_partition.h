#ifndef LF_PLANNING_PASS_FLOW_PARTITION
#define LF_PLANNING_PASS_FLOW_PARTITION

#include "../../core/flow.h"
#include "../../core/memory/adapters.h"
#include "../../core/passes.h"

#ifndef LF_PASS_FLOW_PARTITION_FLOW_QTT_IN_BLOCK
#define LF_PASS_FLOW_PARTITION_FLOW_QTT_IN_BLOCK 256
#endif

void lf_partition_flows(LF_PassPipeline *pipeline);

#endif
