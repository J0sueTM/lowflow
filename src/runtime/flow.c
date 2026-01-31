#include "./flow.h"

void lf_eval_flow(LF_Flow *flow) {
  assert(flow);

  lf_log_debug(&flow->logger, "eval_flow: beg. flow=%x", flow);

  LF_Value **cur_val = (LF_Value **)lf_get_first_list_elem(&flow->val_schedule);
  while (cur_val) {
    switch ((*cur_val)->type) {
      case LF_INT:
      case LF_FLOAT:
      case LF_CHAR:
      case LF_STR:
      case LF_BOOL: {
        LF_Value **frame_val =
          (LF_Value **)lf_alloc_stack_elem(&flow->frame_vals);
        *frame_val = *cur_val;
      } break;
      case LF_FUNC_CALL: {
        // TODO: Maybe reuse old vals?
        LF_Value *new_res_val =
          (LF_Value *)lf_alloc_stack_elem(&flow->new_vals);
        LF_Value *func_def = (*cur_val)->func_call_spec.func_def;
        if (!func_def->func_def_spec->native_impl) {
          lf_log_fatal(&flow->logger,
                       "eval flow: func has no native impl "
                       "[flow=%x, "
                       "def=%x]",
                       flow,
                       func_def);
        }

        size_t func_arg_qtt = func_def->func_def_spec->arg_qtt;
        size_t frame_val_qtt = flow->frame_vals.elem_count;
        if (frame_val_qtt < func_arg_qtt) {
          lf_log_fatal(&flow->logger,
                       "eval_flow: not enough vals in frame for func call. "
                       "flow=%x, "
                       "expected=%d, got=%d",
                       flow,
                       func_arg_qtt,
                       frame_val_qtt);
          // TODO: Stuff above the flow expects results.
          // This should return an Option instead of stopping
          // everything.
          goto finish_eval;
        }

        func_def->func_def_spec->native_impl(new_res_val, &flow->frame_vals);
        lf_reset_stack(&flow->frame_vals);
        LF_Value **res_in_frame =
          (LF_Value **)lf_alloc_stack_elem(&flow->frame_vals);
        *res_in_frame = new_res_val;
      } break;
      case LF_SYMBOL:
      case LF_LIST:
      case LF_SEQ:
      case LF_DICT:
      default:
        assert(false && "not implemented");
        break;
    }
    cur_val =
      (LF_Value **)lf_get_next_list_elem(&flow->val_schedule, (char *)cur_val);
  }

finish_eval:
  lf_log_debug(&flow->logger, "eval_flow: end. flow=%x", flow);
}
