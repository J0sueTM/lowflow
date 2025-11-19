#include "./passes.h"

void lf_run_passes(LF_List *passes, LF_PassContext *ctx) {
  assert(passes);

  lf_init_logger(&ctx->logger);

  LF_Pass *cur_pass = (LF_Pass *)lf_get_first_list_elem(passes);
  while (cur_pass) {
    lf_log_debug(&ctx->logger, "beg pass [p=%s]", cur_pass->name);
    cur_pass->run(ctx);
    lf_log_debug(&ctx->logger, "end pass [p=%s]", cur_pass->name);

    cur_pass = (LF_Pass *)lf_get_next_list_elem(passes, (char *)cur_pass);
  }
}
