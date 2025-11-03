#ifndef LF_RUNTIME_H
#define LF_RUNTIME_H

#include "./types.h"
#include "./memory/list.h"
#include "./memory/stack.h"
#include "./logger.h"

typedef struct LF_Flow {
  LF_Logger logger;
  LF_List val_schedule;
  LF_Stack frame_vals;
  LF_Stack new_vals;
} LF_Flow;

#ifndef LF_FLOW_FRAME_VAL_QTT_IN_BLOCK
#define LF_FLOW_FRAME_VAL_QTT_IN_BLOCK 256
#endif

#ifndef LF_FLOW_NEW_VAL_QTT_IN_BLOCK
#define LF_FLOW_NEW_VAL_QTT_IN_BLOCK 256
#endif

#ifndef LF_FLOW_SCHEDULE_VAL_QTT_IN_BLOCK
#define LF_FLOW_SCHEDULE_VAL_QTT_IN_BLOCK 256
#endif

void lf_init_flow(LF_Flow *flow, LF_Value *entrypoint);
void lf_eval_flow(LF_Flow *flow);

#endif // LF_RUNTIME_H
