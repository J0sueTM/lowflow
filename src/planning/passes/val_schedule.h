#ifndef LF_PLANNING_PASS_VAL_SCHEDULE
#define LF_PLANNING_PASS_VAL_SCHEDULE

#include "../../core/flow.h"
#include "./passes.h"

void lf_reverse_val_schedule(
  LF_Flow *flow,
  LF_Stack *reverse_val_schedule
);

void lf_build_val_schedule(LF_Flow *flow, LF_Value *entrypoint);

void lf_build_val_schedule_new(LF_PassContext *ctx);

#endif
