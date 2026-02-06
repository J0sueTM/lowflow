#include <stdalign.h>

#include "./core/memory/stack.h"
#include "./core/types.h"
#include "./planning/passes/flow_partition.h"
#include "./core/passes.h"
#include "./planning/passes/val_schedule.h"
#include "./runtime/flow.h"
#include "compiler/passes/lexing.h"

// TODO: return error.
void plus_fn_native_impl(LF_Value *out, LF_Stack *frame_vals) {
  LF_Value *snd_arg_val = *(LF_Value **)lf_pop_from_stack(frame_vals);
  LF_Value *fst_arg_val = *(LF_Value **)lf_pop_from_stack(frame_vals);

  int fst = fst_arg_val->as_int;
  int snd = snd_arg_val->as_int;
  int res = fst + snd;

  out->type = LF_INT;
  out->as_int = res;
}

int main(void) {
  // +(+(+(4, 5), 7), 7)
  //       +
  //     +   7
  //   +   7
  // 4   5
  LF_Type plus_fn_arg_types[] = {LF_INT, LF_INT};
  char *plus_fn_arg_names[] = {"fst", "snd"};
  LF_FuncDefSpec plus_fn_def = {
    .arg_types = plus_fn_arg_types,
    .arg_names = plus_fn_arg_names,
    .arg_qtt = 2,
    .ret_type = LF_INT,
    .native_impl = plus_fn_native_impl,
  };
  LF_Value plus_fn = {
    .type = LF_FUNC_DEF,
    .name = "+",
    .func_def_spec = &plus_fn_def,
  };

  LF_Value int_4 = {.type = LF_INT, .as_int = 4};
  LF_Value int_5 = {.type = LF_INT, .as_int = 5};

  LF_Value *left_plus_fn_call_args[] = {&int_4, &int_5};
  LF_Value left_plus_fn_call = {
    .type = LF_FUNC_CALL,
    .func_call_spec = { .func_def = &plus_fn },
    .inner_vals = left_plus_fn_call_args,
  };

  LF_Value int_7 = {.type = LF_INT, .as_int = 7};

  LF_Value *middle_plus_fn_call_args[] = {&left_plus_fn_call, &int_7};
  LF_Value middle_plus_fn_call = {
    .type = LF_FUNC_CALL,
    .func_call_spec = { .func_def = &plus_fn },
    .inner_vals = middle_plus_fn_call_args,
  };

  LF_Value *parent_plus_fn_call_args[] = {
    &middle_plus_fn_call,
    &int_7,
  };
  LF_Value parent_plus_fn_call = {
    .type = LF_FUNC_CALL,
    .func_call_spec = { .func_def = &plus_fn },
    .inner_vals = parent_plus_fn_call_args,
  };

  LF_PassPipeline pipeline;
  pipeline.logger.min_level = LF_DEBUG;
  pipeline.logger.time_fmt = NULL;
  pipeline.entrypoint = &parent_plus_fn_call;
  pipeline.flow_partition_strategy = LF_FLOW_PARTITION_STRATEGY_NONE;
  lf_init_pass_pipeline(&pipeline);
  lf_push_pass(&pipeline, "lexing", lf_lex);
  lf_push_pass(&pipeline, "val_schedule", lf_build_val_schedule);
  lf_push_pass(&pipeline, "flow_partition", lf_partition_flows);
  lf_process_pass_pipeline(&pipeline);

  // lf_debug_stack(&pipeline.val_schedule, lf_debug_value_from_raw);

  // lf_eval_flow(pipeline.parent_flow);

  // LF_Value *res_val = *(LF_Value **)lf_pop_from_stack(&pipeline.parent_flow->frame_vals);
  // printf("res_val = %d\n", res_val->as_int);

  return 0;
}
