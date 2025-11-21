#include "./passes.h"

void lf_init_pass_pipeline(
  LF_PassPipeline *pipeline,
  LF_Value *entrypoint
) {
  assert(pipeline);
  assert(entrypoint);

  pipeline->entrypoint = entrypoint;

  lf_init_logger(&pipeline->logger);

  pipeline->passes.elem_size = sizeof(LF_Pass);
  pipeline->passes.elem_alignment = alignof(LF_Pass);
  pipeline->passes.elem_qtt_in_block = LF_PASS_PIPELINE_PASS_QTT;
  lf_init_list(&pipeline->passes);

  pipeline->val_schedule.elem_size = sizeof(LF_Value *);
  pipeline->val_schedule.elem_qtt_in_block = 1;
  pipeline->val_schedule.elem_alignment = alignof(LF_Value *);
  lf_init_stack(&pipeline->val_schedule);
}

LF_Pass *lf_build_pass(
  LF_PassPipeline *pipeline,
  const char *name,
  void (*apply_fn)(LF_PassPipeline *pipeline)
) {
  assert(pipeline);
  assert(name);
  assert(apply_fn);

  LF_Pass *pass = (LF_Pass *)lf_alloc_list_elem(&pipeline->passes);
  pass->apply_fn = apply_fn;

  size_t name_size = strlen(name);
  size_t name_padded_size = name_size + 1;
  pass->name.char_qtt_in_block = name_padded_size;
  lf_init_string(&pass->name);
  char *pass_name = lf_alloc_string(&pass->name, name_size);
  strncpy(pass_name, name, name_size);
  pass_name[name_padded_size] = '\0';

  return pass;
}

void lf_process_pass_pipeline(LF_PassPipeline *pipeline) {
  assert(pipeline);
  assert(&pipeline->passes);

  LF_Pass *cur_pass = (LF_Pass *)lf_get_first_list_elem(&pipeline->passes);
  while (cur_pass) {
    lf_log_debug(
      &pipeline->logger,
      "process_pass_pipeline: begin pass. pipeline=%p pass=%p pass_name=%s",
      pipeline,
      cur_pass,
      lf_string_to_cstr(&cur_pass->name)
    );

    cur_pass->apply_fn(pipeline);

    lf_log_debug(
      &pipeline->logger,
      "process_pass_pipeline: end pass. pipeline=%p pass=%p pass_name=%s",
      pipeline,
      cur_pass,
      lf_string_to_cstr(&cur_pass->name)
    );

    cur_pass = (LF_Pass *)lf_get_next_list_elem(&pipeline->passes, (char *)cur_pass);
  }
}
