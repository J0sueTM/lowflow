#include <stdio.h>
#include <stdalign.h>

#include "./types.h"
#include "./arena.h"
#include "./runtime.h"

// TODO: return error.
void plus_fn_native_impl(
  LF_Value *out,
  LF_Value *args,
  LF_Value *bindings
) {
  (void)bindings;

  int fst = args[0].as_int;
  int snd = args[0].as_int;
  int res = fst + snd;

  out->type = LF_INT;
  out->as_int = res;
}

int main(void) {
  LF_Type plus_fn_arg_types[] = { LF_INT, LF_INT };
  char *plus_fn_arg_names[] = { "fst", "snd" };

  LF_FuncDefSpec plus_fn_def = {
    .arg_types = plus_fn_arg_types,
    .arg_names = plus_fn_arg_names,
    .arg_qtt = 2,
    .ret_type = LF_INT,
    .native_impl = plus_fn_native_impl
  };

  LF_Value plus_fn = {
    .type = LF_FUNC_DEF,
    .name = "+",
    .func_def_spec = &plus_fn_def,
  };

  LF_Value parent_plus_fn_call_args[] = {
    { .type = LF_INT, .as_int = 1 },
    { .type = LF_INT, .as_int = 2 }
  };
  LF_FuncCallSpec parent_plus_fn_call_spec = {
    .args = parent_plus_fn_call_args
  };
  LF_Value parent_plus_fn_call = {
    .type = LF_FUNC_CALL,
    .func_call_spec = &parent_plus_fn_call_spec,
    .inner_val = &plus_fn
  };

  LF_Value plus_res;
  parent_plus_fn_call.inner_val->func_def_spec->native_impl(
    &plus_res,
    parent_plus_fn_call.func_call_spec->args,
    NULL
  );

  printf("res = %d\n", plus_res.as_int);

  LF_Flow flow;
  lf_build_flow(&flow, &parent_plus_fn_call);

  return 0;
}
