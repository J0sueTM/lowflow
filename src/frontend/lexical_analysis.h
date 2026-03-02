#ifndef LF_FRONTEND_LEXICAL_ANALYSIS
#define LF_FRONTEND_LEXICAL_ANALYSIS

#include <stdlib.h>

#include "../core/memory/list.h"
#include "../core/memory/string.h"

typedef enum LF_TokenType {
  LF_TOKEN_UNKNOWN,

  // Structure.
  LF_TOKEN_IDENTIFIER,
  LF_TOKEN_STR,

  // Keywords.
  LF_TOKEN_USE,
  LF_TOKEN_ENTRY,
  LF_TOKEN_FUNC,

  // Punctuations.
  LF_TOKEN_EQUAL, // =
  LF_TOKEN_SLASH, // /
  LF_TOKEN_COLON, // :
  LF_TOKEN_BANG,  // !

  // Scope delimiters.
  LF_TOKEN_PAREN_BEG, // (
  LF_TOKEN_PAREN_END, // )
  LF_TOKEN_BRACE_BEG, // {
  LF_TOKEN_BRACE_END, // }
  LF_TOKEN_EOF
} LF_TokenType;

typedef struct LF_Token {
  LF_TokenType type;
  char *lexeme;
  size_t start_pos;
  size_t end_pos;
} LF_Token;

typedef struct LF_LexedSource {
  LF_List tokens;
  char *lexemes;
  LF_String source;
} LF_LexedSource;

void lf_lex(LF_List *tokens, LF_String *lexemes, char *source, size_t size);

#endif // LF_FRONTEND_LEXICAL_ANALYSIS
