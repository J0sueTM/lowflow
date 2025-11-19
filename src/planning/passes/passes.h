#ifndef LF_PLANNING_PASS
#define LF_PLANNING_PASS

#include "../../core/logger.h"
#include "../../core/types.h"
#include "../../core/memory/list.h"

typedef struct LF_PassContext {
  LF_Logger logger;
  LF_Value *entrypoint;
  LF_Stack val_schedule;
} LF_PassContext;

typedef struct LF_Pass {
  LF_String name;
  void (*run)(LF_PassContext *ctx);
} LF_Pass;

void lf_run_passes(LF_List *passes, LF_PassContext *ctx);

#endif
