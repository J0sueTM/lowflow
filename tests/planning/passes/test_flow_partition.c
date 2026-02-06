#include "../../../src/core/memory/stack.h"
#include "../../../src/core/types.h"
#include "../../../src/planning/passes/flow_partition.h"
#include "../../../vendor/munit/munit.h"

static MunitResult test_partition_flows__no_strategy(
  const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  LF_Value dumb_values[6] = {0};

  LF_PassPipeline pipeline = {
    .flow_partition_strategy = LF_FLOW_PARTITION_STRATEGY_NONE,
    .entrypoint = dumb_values,
  };
  lf_init_pass_pipeline(&pipeline);

  for (int i = 0; i < 6; ++i) {
    LF_Value *dumb_value = &dumb_values[i];
    LF_Value **scheduled_val = (LF_Value **)lf_alloc_stack_elem(&pipeline.val_schedule);
    *scheduled_val = dumb_value;
  }

  lf_partition_flows(&pipeline);

  LF_Value *fst_scheduled_val = *(LF_Value **)lf_get_first_list_elem(&pipeline.parent_flow->val_schedule);
  munit_assert_ptr(&dumb_values[5], ==, fst_scheduled_val);

  LF_Value *last_scheduled_val = *(LF_Value **)lf_get_last_list_elem(&pipeline.parent_flow->val_schedule);
  munit_assert_ptr(&dumb_values[0], ==, last_scheduled_val);

  return MUNIT_OK;
}

static MunitTest flow_partition_pass_tests[] = {
  {
    .name = "/no_strategy",
    .test = test_partition_flows__no_strategy,
    .setup = NULL,
    .tear_down = NULL,
    .options = MUNIT_TEST_OPTION_NONE,
    .parameters = NULL,
  },
  {0},
};

static const MunitSuite flow_partition_pass_test_suite = {
  .prefix = "/flow_partition",
  .tests = flow_partition_pass_tests,
  .iterations = 1,
  .options = MUNIT_SUITE_OPTION_NONE,
};
