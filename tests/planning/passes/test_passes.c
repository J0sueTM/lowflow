#include "../../../vendor/munit/munit.h"
#include "../../../src/planning/passes/passes.h"

static void _pass_fn__change_val_schedule(LF_PassContext *ctx) {
  lf_alloc_stack_elem(&ctx->val_schedule);
}

static MunitResult test_passes__success(
  const MunitParameter params[],
  void *data
) {
  LF_List passes = {
    .elem_size = sizeof(LF_Pass),
    .elem_alignment = alignof(LF_Pass),
    .elem_qtt_in_block = 3
  };
  lf_init_list(&passes);

  LF_Pass *new_pass = (LF_Pass *)lf_alloc_list_elem(&passes, 1);
  new_pass->name.char_qtt_in_block = 10;
  lf_init_string(&new_pass->name);
  char *name = lf_alloc_string(&new_pass->name, 10);
  strcpy(name, "first pass");
  new_pass->run = _pass_fn__change_val_schedule;

  LF_PassContext ctx;

  ctx.val_schedule.elem_size = sizeof(LF_Value *);
  ctx.val_schedule.elem_qtt_in_block = 1;
  ctx.val_schedule.elem_alignment = alignof(LF_Value *);
  lf_init_stack(&ctx.val_schedule);
  
  lf_run_passes(&passes, &ctx);

  munit_assert_size(ctx.val_schedule.elem_count, ==, 1);

  return MUNIT_OK;
}

static MunitTest passes_tests[] = {
  {
    .name = "/success",
    .test = test_passes__success,
    .setup = NULL,
    .tear_down = NULL,
    .options = MUNIT_TEST_OPTION_NONE,
    .parameters = NULL
  },
  { 0 },
};

static const MunitSuite passes_test_suite = {
  .prefix = "/passes",
  .tests = passes_tests,
  .iterations = 1,
  .options = MUNIT_SUITE_OPTION_NONE,
};
