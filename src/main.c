#include <stdalign.h>

#include "./core/memory/stack.h"
#include "./core/types.h"
#include "./planning/passes/flow_partition.h"
#include "./planning/passes/passes.h"
#include "./planning/passes/val_schedule.h"

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

  LF_Value child_plus_fn_call_args[] = {
    {.type = LF_INT, .as_int = 4},
    {.type = LF_INT, .as_int = 5},
  };
  LF_FuncCallSpec child_plus_fn_call_spec = {
    .args = child_plus_fn_call_args,
  };
  LF_Value child_plus_fn_call = {
    .type = LF_FUNC_CALL,
    .func_call_spec = &child_plus_fn_call_spec,
    .inner_val = &plus_fn,
  };

  LF_Value parent_plus_fn_call_args[] = {
    child_plus_fn_call,
    {.type = LF_INT, .as_int = 2},
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
  lf_push_pass(&pipeline, "flow_partition", lf_partition_flows);
  lf_process_pass_pipeline(&pipeline);

  return 0;
}
