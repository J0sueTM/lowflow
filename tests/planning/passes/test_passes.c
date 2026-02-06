#include "../../../src/core/passes.h"
#include "../../../vendor/munit/munit.h"
#include "./test_flow_partition.c"
#include "./test_val_schedule.c"

static void _pass_fn__change_val_schedule(LF_PassPipeline *ctx) {
  lf_alloc_stack_elem(&ctx->val_schedule);
}

static MunitResult test_pass_pipeline__success(const MunitParameter params[],
                                               void *data) {
  (void)params;
  (void)data;

  LF_Value dumb_value = {0};

  LF_PassPipeline pipeline;
  pipeline.entrypoint = &dumb_value;
  lf_init_pass_pipeline(&pipeline);
  lf_push_pass(&pipeline, "fst_pass", _pass_fn__change_val_schedule);
  lf_push_pass(&pipeline, "snd_pass", _pass_fn__change_val_schedule);

  lf_process_pass_pipeline(&pipeline);

  munit_assert_size(pipeline.val_schedule.elem_count, ==, 2);

  return MUNIT_OK;
}

static MunitTest planning_pass_pipeline_tests[] = {
  {
    .name = "/pipeline/success",
    .test = test_pass_pipeline__success,
    .setup = NULL,
    .tear_down = NULL,
    .options = MUNIT_TEST_OPTION_NONE,
    .parameters = NULL,
  },
  {0},
};

static MunitSuite planning_passes_inner_test_suites[] = {
  val_schedule_pass_test_suite,
  flow_partition_pass_test_suite,
  {0},
};

static const MunitSuite planning_passes_test_suite = {
  .prefix = "/passes",
  .tests = planning_pass_pipeline_tests,
  .iterations = 1,
  .options = MUNIT_SUITE_OPTION_NONE,
  .suites = planning_passes_inner_test_suites,
};
