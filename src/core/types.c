#include "./types.h"

void lf_debug_value(LF_Value *val) {
  assert(val);

  switch (val->type) {
  case LF_INT:
    printf("%d ", val->as_int);
    break;
  case LF_FUNC_CALL:
    printf("%s ", val->func_call_spec.func_def->name);
    break;
  default:
    assert(NULL && "not implented!");
    break;
  }
}

void lf_debug_value_from_raw(char *data) {
  assert(data);

  LF_Value **val = (LF_Value **)data;
  lf_debug_value(*val);
}
