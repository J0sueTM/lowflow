#ifndef LF_FLOW
#define LF_FLOW

#include "./logger.h"
#include "./memory/list.h"
#include "./memory/stack.h"
#include "./types.h"

#ifndef LF_FLOW_FRAME_VAL_QTT_IN_BLOCK
#define LF_FLOW_FRAME_VAL_QTT_IN_BLOCK 256
#endif

#ifndef LF_FLOW_NEW_VAL_QTT_IN_BLOCK
#define LF_FLOW_NEW_VAL_QTT_IN_BLOCK 256
#endif

#ifndef LF_FLOW_SCHEDULE_VAL_QTT_IN_BLOCK
#define LF_FLOW_SCHEDULE_VAL_QTT_IN_BLOCK 256
#endif

#ifndef LF_FLOW_LOGGER_MIN_LEVEL
#define LF_FLOW_LOGGER_MIN_LEVEL LF_DEBUG
#endif

typedef enum LF_FlowPartitionStrategy {
  LF_FLOW_PARTITION_STRATEGY_NONE,
  LF_FLOW_PARTITION_STRATEGY_VAL_COUNT
} LF_FlowPartitionStrategy;

typedef struct LF_Flow {
  LF_Logger logger;
  LF_List val_schedule;
  LF_Stack frame_vals;
  LF_Stack new_vals;
} LF_Flow;

void lf_init_flow(LF_Flow *flow);

#endif
