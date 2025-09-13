#include <stdio.h>

#include "./types.h"

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
    .ret_type = LF_INT,
    .native_impl = plus_fn_native_impl
  };

  LF_Value plus_fn = {
    .name = "+",
    .func_def_spec = &plus_fn_def,
  };

  LF_Value plus_fn_call_args[] = {
    { .type = LF_INT, .as_int = 1 },
    { .type = LF_INT, .as_int = 2 }
  };
  LF_FuncCallSpec plus_fn_call_spec = {
    .args = plus_fn_call_args
  };
  LF_Value plus_fn_call = {
    .func_call_spec = &plus_fn_call_spec,
    .inner_val = &plus_fn
  };

  LF_Value plus_res;
  plus_fn_call.inner_val->func_def_spec->native_impl(
    &plus_res,
    plus_fn_call.func_call_spec->args,
    NULL
  );

  printf("%d\n", plus_res.as_int);

  return 0;
}
