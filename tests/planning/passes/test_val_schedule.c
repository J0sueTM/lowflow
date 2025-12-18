#include "../../../src/core/memory/stack.h"
#include "../../../src/planning/passes/val_schedule.h"
#include "../../../vendor/munit/munit.h"

static void _plus_fn_native_impl(LF_Value *out, LF_Stack *frame_vals) {
  LF_Value *snd_arg_val = *(LF_Value **)lf_pop_from_stack(frame_vals);
  LF_Value *fst_arg_val = *(LF_Value **)lf_pop_from_stack(frame_vals);

  int fst = fst_arg_val->as_int;
  int snd = snd_arg_val->as_int;
  int res = fst + snd;

  out->type = LF_INT;
  out->as_int = res;
}

static MunitResult test_build_val_schedule__success(
  const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  LF_Type plus_fn_arg_types[] = {LF_INT, LF_INT};
  char *plus_fn_arg_names[] = {"fst", "snd"};
  LF_FuncDefSpec plus_fn_def = {
    .arg_types = plus_fn_arg_types,
    .arg_names = plus_fn_arg_names,
    .arg_qtt = 2,
    .ret_type = LF_INT,
    .native_impl = _plus_fn_native_impl,
  };

  LF_Value plus_fn = {
    .type = LF_FUNC_DEF,
    .name = "+",
    .func_def_spec = &plus_fn_def,
  };

  LF_Value int_4 = {.type = LF_INT, .as_int = 4};
  LF_Value int_2 = {.type = LF_INT, .as_int = 2};

  LF_Value plus_fn_call_args[] = {int_4, int_2};
  LF_FuncCallSpec plus_fn_call_spec = {
    .args = plus_fn_call_args,
  };
  LF_Value plus_fn_call = {
    .type = LF_FUNC_CALL,
    .func_call_spec = &plus_fn_call_spec,
    .inner_val = &plus_fn,
  };

  LF_PassPipeline pipeline;
  lf_init_pass_pipeline(&pipeline, &plus_fn_call);

  lf_build_val_schedule(&pipeline);

  LF_Value *fst_val = *(LF_Value **)lf_pop_from_stack(&pipeline.val_schedule);
  munit_assert_int(fst_val->as_int, ==, int_4.as_int);

  LF_Value *snd_val = *(LF_Value **)lf_pop_from_stack(&pipeline.val_schedule);
  munit_assert_int(snd_val->as_int, ==, int_2.as_int);

  LF_Value *thd_val = *(LF_Value **)lf_pop_from_stack(&pipeline.val_schedule);
  munit_assert_ptr_equal(thd_val, &plus_fn_call);

  return MUNIT_OK;
}

static MunitTest val_schedule_pass_tests[] = {
  {.name = "/build",
   .test = test_build_val_schedule__success,
   .setup = NULL,
   .tear_down = NULL,
   .options = MUNIT_TEST_OPTION_NONE,
   .parameters = NULL},
  {0},
};

static const MunitSuite val_schedule_pass_test_suite = {
  .prefix = "/val_schedule",
  .tests = val_schedule_pass_tests,
  .iterations = 1,
  .options = MUNIT_SUITE_OPTION_NONE,
};
