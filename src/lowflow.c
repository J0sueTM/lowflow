#include <stdio.h>
#include <stdalign.h>

#include "./types.h"
#include "./arena.h"

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

  printf("res = %d\n", plus_res.as_int);

  LF_Arena arena = { .block_size = 2 * sizeof(double) };
  lf_init_arena(&arena);
  double *d1 = (double *)lf_arena_alloc(&arena, sizeof(double), alignof(double));
  double *d2 = (double *)lf_arena_alloc(&arena, sizeof(double), alignof(double));
  double *d3 = (double *)lf_arena_alloc(&arena, sizeof(double), alignof(double));
  printf("double = %f %f %f\n", *d1, *d2, *d3);
  printf("block count = %ld\n", arena.block_count);

  return 0;
}
