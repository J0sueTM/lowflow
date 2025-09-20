#ifndef LF_RUNTIME_H
#define LF_RUNTIME_H

#include "./types.h"
#include "./arena.h"

typedef struct LF_Flow {
  LF_Arena vals_exec_seq;
  size_t cur_pos;
} LF_Flow;

size_t lf_init_flow(LF_Flow *flow, LF_Value *entrypoint);

#endif // LF_RUNTIME_H
