#ifndef LF_LOGGER_H
#define LF_LOGGER_H

#include <stdarg.h>
#include <time.h>

#include "./memory/string.h"

typedef enum LF_LogLevel {
  LF_DEBUG = 0,
  LF_INFO = 1,
  LF_WARNING = 2,
  LF_ERROR = 3,
  LF_FATAL = 4
} LF_LogLevel;

#ifndef LF_LOGGER_BUF_CHAR_QTT_IN_BLOCK
#define LF_LOGGER_BUF_CHAR_QTT_IN_BLOCK 512
#endif

typedef struct LF_Logger {
  LF_LogLevel min_level;
  LF_String buf;
  const char *time_fmt;
  bool is_colored;
} LF_Logger;

#ifdef LF_DEBUG_CONFIG
#define lf_log_debug(logger, ...) \
  lf_log(logger, LF_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#else
#define lf_log_debug(...) \
  {                       \
  }
#endif

#define lf_log_info(logger, ...) \
  lf_log(logger, LF_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define lf_log_warning(logger, ...) \
  lf_log(logger, LF_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define lf_log_error(logger, ...) \
  lf_log(logger, LF_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define lf_log_fatal(logger, ...) \
  lf_log(logger, LF_FATAL, __FILE__, __LINE__, __VA_ARGS__)

void lf_init_logger(LF_Logger *logger);

void lf_log(LF_Logger *logger,
            LF_LogLevel level,
            const char *fname,
            int line,
            const char *fmt,
            ...);

#endif // LF_LOGGER_H
