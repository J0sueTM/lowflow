#include "./logger.h"

#define LF_LEVEL_STR_SIZE 3
#define LF_COLOR_PREFIX_SIZE 4   // "\033[xx"
#define LF_COLOR_SUFFIX_SIZE 1   // "m"
#define LF_COLOR_RESET_SIZE 4    // "\033[0m"
#define LF_COLORED_LEVEL_STR_SIZE (                   \
    LF_COLOR_PREFIX_SIZE + LF_COLOR_SUFFIX_SIZE +     \
    LF_LEVEL_STR_SIZE + LF_COLOR_RESET_SIZE           \
  )

static const char level_strs[][LF_LEVEL_STR_SIZE] = {
  {'D', 'B', 'G'},
  {'I', 'N', 'F'},
  {'W', 'A', 'R'},
  {'E', 'R', 'R'},
  {'F', 'A', 'T'},
};

static const char colored_level_strs[][LF_COLORED_LEVEL_STR_SIZE] = {
  /* DBG – gray */
  { '\033','[','9','0','m','D','B','G','\033','[','0','m' },
  /* INF – green */
  { '\033','[','3','2','m','I','N','F','\033','[','0','m' },
  /* WAR – yellow */
  { '\033','[','3','3','m','W','A','R','\033','[','0','m' },
  /* ERR – red */
  { '\033','[','3','1','m','E','R','R','\033','[','0','m' },
  /* FAT – magenta */
  { '\033','[','3','5','m','F','A','T','\033','[','0','m' },
};

void lf_init_logger(LF_Logger *logger) {
  assert(logger);

  logger->buf.char_qtt_in_block = LF_LOGGER_BUF_CHAR_QTT_IN_BLOCK;
  lf_init_string(&logger->buf);

  if (!logger->time_fmt) {
    logger->time_fmt = "%Y-%m-%d %H:%M:%S";
  }
}

void lf_log(
  LF_Logger *logger,
  LF_LogLevel level,
  const char *fname,
  int line,
  const char *fmt,
  ...
) {
  assert(logger);
  
  if (level < logger->min_level) {
    return;
  }

  lf_reset_string(&logger->buf);

  LF_MemBlock *head_block = logger->buf.arena.head_block;
  assert(head_block);

  time_t now = time(NULL);
  struct tm *now_info = localtime(&now);
  size_t time_size = strftime(
    head_block->data,
    logger->buf.char_qtt_in_block,
    logger->time_fmt,
    now_info
  );
  if (time_size == 0) {
    strcpy(head_block->data, "0000-00-00 00:00:00");
    time_size = 19;
  }
  lf_alloc_string(&logger->buf, time_size);

  size_t level_str_size = (
    (logger->is_colored)
      ? LF_COLORED_LEVEL_STR_SIZE
      : LF_LEVEL_STR_SIZE
  );
  const char *level_str = (
    (logger->is_colored)
      ? colored_level_strs[level]
      : level_strs[level]
  );
  size_t padded_level_str_size = level_str_size + 2;
  char *level_buf = lf_alloc_string(
    &logger->buf,
    padded_level_str_size
  );
  level_buf[0] = ' ';
  memcpy(level_buf + 1, level_str, level_str_size);
  level_buf[padded_level_str_size - 1] = ' ';

  va_list ap;
  va_start(ap, fmt);

  // TODO: Split big string between blocks.
  char *msg_buf = head_block->data + head_block->offset;
  size_t remaining_size = head_block->size - head_block->offset;
  size_t msg_needed_size = vsnprintf(
    msg_buf,
    remaining_size,
    fmt,
    ap
  );
  assert(msg_needed_size + 1 < remaining_size);
  msg_buf[msg_needed_size] = '\n';
  msg_buf[msg_needed_size + 1] = '\0';
  lf_alloc_string(&logger->buf, msg_needed_size + 1);

  va_end(ap);

  fprintf(stderr, "%s", head_block->data);
}
