#include "./flow.h"

void lf_init_flow(LF_Flow *flow) {
  assert(flow);

  flow->logger.min_level = LF_FLOW_LOGGER_MIN_LEVEL;
  flow->logger.time_fmt = NULL;
  lf_init_logger(&flow->logger);
}
