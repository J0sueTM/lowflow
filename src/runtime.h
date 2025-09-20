#ifndef LF_RUNTIME_H
#define LF_RUNTIME_H

#include "./types.h"
#include "./arena.h"

typedef struct LF_Flow {
  LF_Value **vals;
  size_t cur_pos;
} LF_Flow;

void lf_init_flow(LF_Flow *flow);
void lf_build_flow(LF_Flow *flow, LF_Value *entrypoint);

#endif // LF_RUNTIME_H
