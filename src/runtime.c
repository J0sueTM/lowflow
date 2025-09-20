#include "./runtime.h"

size_t lf_init_flow(LF_Flow *flow, LF_Value *entrypoint) {
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

  size_t cur_val_pos = 0;
  LF_Value *cur_val = NULL;
  bool should_stop = false;
  while (!should_stop) {
    cur_val = *(LF_Value **)lf_arena_get_elem_by_pos(
      &visited_vals,
      cur_val_pos,
      sizeof(LF_Value *),
      alignof(LF_Value *)
    );
    if (!cur_val) {
      should_stop = true;
      continue;
    }
    ++cur_val_pos;

    bool is_leaf = (cur_val->type & PRIMITIVE_MASK) != 0;
    if (is_leaf) {
      continue;
    }

    size_t arg_qtt = cur_val->inner_val->func_def_spec->arg_qtt;
    LF_Value **arg_val_ptrs = (LF_Value **)lf_arena_alloc(
      &visited_vals, arg_qtt * sizeof(LF_Value *),
      alignof(LF_Value *)
    );
    for (size_t i = 0; i < arg_qtt; ++i) {
      arg_val_ptrs[i] = &cur_val->func_call_spec->args[i];
    }
  }

  size_t val_qtt = cur_val_pos;
  flow->vals_exec_seq.block_size = val_qtt * sizeof(LF_Value *);
  lf_init_arena(&flow->vals_exec_seq);
  for (size_t i = val_qtt; i >= 1; --i) {
    LF_Value **val_exec_ptr = (LF_Value **)lf_arena_alloc(
      &flow->vals_exec_seq, sizeof(LF_Value *), alignof(LF_Value *)
    );
    *val_exec_ptr = *(LF_Value **)lf_arena_get_elem_by_pos(
      &visited_vals, (i - 1), sizeof(LF_Value *), alignof(LF_Value *)
    );
    assert(*val_exec_ptr);
  }

  return val_qtt;
}
