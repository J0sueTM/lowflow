#include "./runtime.h"

void lf_init_flow(LF_Flow *flow) {
  assert(flow);

  flow->cur_pos = 0;
}

void lf_build_flow(LF_Flow *flow, LF_Value *entrypoint) {
  assert(flow);
  assert(entrypoint);

  #ifndef LF_FLOW_VISITED_VAL_QTT_IN_BLOCK
  #define LF_FLOW_VISITED_VAL_QTT_IN_BLOCK 500
  #endif

  size_t block_size = LF_FLOW_VISITED_VAL_QTT_IN_BLOCK * sizeof(LF_Value *);
  LF_Arena visited_vals = { .block_size = block_size };
  lf_init_arena(&visited_vals);

  LF_Value **entrypoint_ptr = (LF_Value **)lf_arena_alloc(
    &visited_vals, sizeof(LF_Value *), alignof(LF_Value *)
  );
  *entrypoint_ptr = entrypoint;

  LF_Value *cur_val = entrypoint;
  bool should_stop = false;
  while (!should_stop) {
    switch (cur_val->type) {
    case LF_INT:
    case LF_FLOAT:
    case LF_CHAR:
    case LF_STR:
    case LF_BOOL:
    case LF_FUNC_DEF:
    case LF_TYPE:
    case LF_TRAIT:
      // assert(false && "primitive value not implemented");
      break;
    case LF_FUNC_CALL: {
      size_t arg_qtt = cur_val->inner_val->func_def_spec->arg_qtt;
      LF_Value **arg_val_ptrs = (LF_Value **)lf_arena_alloc(
        &visited_vals, arg_qtt * sizeof(LF_Value *),
        alignof(LF_Value *)
      );
      for (size_t i = 0; i < arg_qtt; ++i) {
        arg_val_ptrs[i] = &cur_val->func_call_spec->args[i];
      }
      cur_val = arg_val_ptrs[arg_qtt - 1];
    } break;
    default:
      assert(false && "unknown value type");
      break;
    }
  }
}
