#ifndef LF_FRONTEND_LEXICAL_ANALYSIS
#define LF_FRONTEND_LEXICAL_ANALYSIS

#include <stdlib.h>

#include "../core/memory/list.h"
#include "../core/memory/string.h"

typedef enum LF_TokenType {
  LF_TOKEN_UNKNOWN = 1 << 0,

  // Structure.
  LF_TOKEN_IDENTIFIER = 1 << 1,
  LF_TOKEN_STR = 1 << 2,

  // Keywords.
  LF_TOKEN_USE = 1 << 3,
  LF_TOKEN_ENTRY = 1 << 4,
  LF_TOKEN_FUNC = 1 << 5,

  // Punctuations.
  LF_TOKEN_COMMA = 1 << 6, // ,
  LF_TOKEN_EQUAL = 1 << 7, // =
  LF_TOKEN_SLASH = 1 << 8, // /
  LF_TOKEN_COLON = 1 << 9, // :
  LF_TOKEN_BANG = 1 << 10, // !

  // Scope delimiters.
  LF_TOKEN_PAREN_BEG = 1 << 11, // (
  LF_TOKEN_PAREN_END = 1 << 12, // )
  LF_TOKEN_BRACE_BEG = 1 << 13, // {
  LF_TOKEN_BRACE_END = 1 << 14, // }
  LF_TOKEN_EOF = 1 << 15
} LF_TokenType;

typedef struct LF_Token {
  LF_TokenType type;
  LF_String *lexeme;
} LF_Token;

void lf_lex(LF_List *tokens,
            LF_List *lexemes,
            LF_List *lexeme_slices,
            LF_String *source);

#endif // LF_FRONTEND_LEXICAL_ANALYSIS
