#ifndef LF_PLANNING_PASSES
#define LF_PLANNING_PASSES

#include "../../core/logger.h"
#include "../../core/memory/list.h"
#include "../../core/types.h"

#ifndef LF_PASS_PIPELINE_PASS_QTT
#define LF_PASS_PIPELINE_PASS_QTT 4
#endif

typedef struct LF_PassPipeline {
  LF_Logger logger;
  LF_Value *entrypoint;
  LF_Stack val_schedule;
  LF_List passes;
} LF_PassPipeline;

typedef struct LF_Pass {
  LF_String name;
  void (*apply_fn)(LF_PassPipeline *pipeline);
} LF_Pass;

void lf_init_pass_pipeline(LF_PassPipeline *pipeline, LF_Value *entrypoint);

LF_Pass *lf_append_pass(LF_PassPipeline *pipeline,
                        const char *name,
                        void (*apply_fn)(LF_PassPipeline *pipeline));

void lf_process_pass_pipeline(LF_PassPipeline *pipeline);

#endif
