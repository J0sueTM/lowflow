#include <stdalign.h>

#include "./core/memory/stack.h"
#include "./core/types.h"
#include "./planning/passes/flow_partition.h"
#include "./planning/passes/passes.h"
#include "./planning/passes/val_schedule.h"
#include "./runtime/flow.h"

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

  LF_Value *bottom_plus_fn_call_args[] = {
    &int_4,
    &int_5,  
  };
  LF_FuncCallSpec bottom_plus_fn_call_spec = {
    .args = bottom_plus_fn_call_args,
  };
  LF_Value bottom_plus_fn_call = {
    .type = LF_FUNC_CALL,
    .func_call_spec = &bottom_plus_fn_call_spec,
    .inner_val = &plus_fn,
  };

  LF_Value int_7 = {.type = LF_INT, .as_int = 7};

  LF_Value *fst_middle_plus_fn_call_args[] = {
    &bottom_plus_fn_call,
    &int_7,
  };
  LF_FuncCallSpec fst_middle_plus_fn_call_spec = {
    .args = fst_middle_plus_fn_call_args
  };
  LF_Value fst_middle_plus_fn_call = {
    .type = LF_FUNC_CALL,
    .func_call_spec = &fst_middle_plus_fn_call_spec,
    .inner_val = &plus_fn
  };

  LF_Value *middle_plus_fn_calls_args[100][2];

  LF_FuncCallSpec middle_plus_fn_calls_specs[100];
  LF_Value middle_plus_fn_calls[100];
  for (size_t i = 0; i < 100; ++i) {
    LF_Value **cur_call_args = middle_plus_fn_calls_args[i];
    cur_call_args[0] = &int_7;
    cur_call_args[1] = &fst_middle_plus_fn_call;

    LF_FuncCallSpec *cur_call_spec = &middle_plus_fn_calls_specs[i];
    cur_call_spec->args = cur_call_args;

    LF_Value *cur_fn_call = &middle_plus_fn_calls[i];
    cur_fn_call->type = LF_FUNC_CALL;
    cur_fn_call->func_call_spec = cur_call_spec;
    cur_fn_call->inner_val = &plus_fn;
  }

  LF_Value int_2 = {.type = LF_INT, .as_int = 2};

  LF_Value *parent_plus_fn_call_args[] = {
    &middle_plus_fn_calls[99],
    &int_2,
  };
  LF_FuncCallSpec parent_plus_fn_call_spec = {
    .args = parent_plus_fn_call_args,
  };
  LF_Value parent_plus_fn_call = {
    .type = LF_FUNC_CALL,
    .func_call_spec = &parent_plus_fn_call_spec,
    .inner_val = &plus_fn,
  };

  LF_PassPipeline pipeline;
  pipeline.logger.min_level = LF_DEBUG;
  pipeline.logger.time_fmt = NULL;
  lf_init_pass_pipeline(&pipeline, &parent_plus_fn_call);
  lf_push_pass(&pipeline, "val_schedule", lf_build_val_schedule);

  pipeline.flow_partition_strategy = LF_FLOW_PARTITION_STRATEGY_NONE;
  lf_push_pass(&pipeline, "flow_partition", lf_partition_flows);

  lf_process_pass_pipeline(&pipeline);

  lf_eval_flow(pipeline.parent_flow);

  LF_Value *res_val = *(LF_Value **)lf_pop_from_stack(&pipeline.parent_flow->frame_vals);
  printf("res_val = %d\n", res_val->as_int);

  return 0;
}
