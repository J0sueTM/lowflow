#include "./lexical_analysis.h"

static bool _lf_is_whitespace(char c) {
  switch (c) {
    case ' ':
    case '\n':
    case '\r':
    case '\t':
      return true;
    default:
      return false;
  }
}

// Hard boundary means that these lexemes will be flushed wherever they
// appear. So hello{} flushes out 3 tokens, but `hello!` and `hello !`,
// on the other hand flushes out 1 and 2 tokens respectively, since
// they have soft boundaries.
static LF_TokenType _lf_get_token_type_from_hard_boundary_lexeme(char c) {
  switch (c) {
    case '/':
      return LF_TOKEN_SLASH;
      break;
    case '(':
      return LF_TOKEN_PAREN_BEG;
      break;
    case ')':
      return LF_TOKEN_PAREN_END;
      break;
    case '{':
      return LF_TOKEN_BRACE_BEG;
      break;
    case '}':
      return LF_TOKEN_BRACE_END;
      break;
    case ':':
      return LF_TOKEN_COLON;
      break;
    case ',':
      return LF_TOKEN_COMMA;
      break;
    default:
      return LF_TOKEN_UNKNOWN;
      break;
  }
}

static LF_TokenType _lf_get_token_type_from_soft_boundary_lexeme(char c) {
  switch (c) {
    case '=':
      return LF_TOKEN_EQUAL;
      break;
    case '!':
      return LF_TOKEN_BANG;
      break;
    default:
      return LF_TOKEN_UNKNOWN;
      break;
  }
}

static LF_TokenType _lf_get_token_type_from_big_lexeme(LF_String *lexeme) {
  char *char_str = lf_string_to_cstr(lexeme);
  if (lexeme->str_qtt == 3 && memcmp(char_str, "use", 3) == 0) {
    return LF_TOKEN_USE;
  } else if (lexeme->str_qtt == 5 && memcmp(char_str, "entry", 5) == 0) {
    return LF_TOKEN_ENTRY;
  } else if (lexeme->str_qtt == 4 && memcmp(char_str, "func", 4) == 0) {
    return LF_TOKEN_FUNC;
  }
  return LF_TOKEN_IDENTIFIER;
}

static void _lf_copy_lexeme_to_token(LF_Token *dest_token,
                                     LF_String *lexemes,
                                     LF_String *src_lexeme,
                                     size_t src_pos) {
  char *lexeme_buf = lf_alloc_string(lexemes, src_lexeme->str_qtt + 1);
  memcpy(lexeme_buf, lf_string_to_cstr(src_lexeme), src_lexeme->str_qtt);
  lexeme_buf[src_lexeme->str_qtt] = '\0';
  dest_token->lexeme = lexeme_buf;
}

static void _lf_flush_lexeme(LF_List *tokens,
                             LF_String *lexemes,
                             LF_String *cur_lexeme,
                             size_t src_pos) {
  LF_Token *token = (LF_Token *)lf_alloc_list_elem(tokens);
  token->start_pos = src_pos - cur_lexeme->str_qtt;
  token->end_pos = src_pos;

  if (cur_lexeme->str_qtt <= 1) {
    char lexeme_char = *lf_string_to_cstr(cur_lexeme);
    LF_TokenType token_type =
      _lf_get_token_type_from_soft_boundary_lexeme(lexeme_char);
    if (token_type == LF_TOKEN_UNKNOWN) {
      token_type = _lf_get_token_type_from_hard_boundary_lexeme(lexeme_char);
    }
    token->type = token_type;
    token->lexeme = NULL;
  } else {
    token->type = _lf_get_token_type_from_big_lexeme(cur_lexeme);
    if (token->type == LF_TOKEN_IDENTIFIER) {
      _lf_copy_lexeme_to_token(token, lexemes, cur_lexeme, src_pos);
    }
  }

  lf_reset_string(cur_lexeme);
}

static void _lf_push_lexeme_char(LF_String *lexeme, char *c) {
  char *buf = lf_alloc_string(lexeme, 1);
  buf[0] = *c;
}

void lf_lex(LF_List *tokens, LF_String *lexemes, char *source, size_t size) {
  assert(source);
  assert(lexemes);

  LF_String cur_lexeme = {.char_qtt_in_block = 255};
  lf_init_string(&cur_lexeme);

  bool is_reading_str = false;
  for (size_t src_pos = 0; src_pos < size; ++src_pos) {
    char *cur_char = (source + src_pos);

    if (*cur_char == '"' && !is_reading_str) {
      is_reading_str = true;
      continue;
    } else if (*cur_char == '"' && is_reading_str) {
      LF_Token *str_token = (LF_Token *)lf_alloc_list_elem(tokens);
      str_token->type = LF_TOKEN_STR;

      _lf_copy_lexeme_to_token(str_token, lexemes, &cur_lexeme, src_pos);
      lf_reset_string(&cur_lexeme);

      is_reading_str = false;
      continue;
    } else if (is_reading_str) {
      _lf_push_lexeme_char(&cur_lexeme, cur_char);
      continue;
    }

    bool has_lexeme = cur_lexeme.str_qtt >= 1;
    bool is_whitespace = _lf_is_whitespace(*cur_char);
    if (is_whitespace && has_lexeme) {
      _lf_flush_lexeme(tokens, lexemes, &cur_lexeme, src_pos);
      continue;
    } else if (is_whitespace) {
      continue;
    }

    LF_TokenType cur_token_type =
      _lf_get_token_type_from_hard_boundary_lexeme(*cur_char);
    bool is_hard_boundary_token = cur_token_type != LF_TOKEN_UNKNOWN;
    if (is_hard_boundary_token) {
      if (has_lexeme) {
        _lf_flush_lexeme(tokens, lexemes, &cur_lexeme, src_pos);
      }

      LF_Token *token = (LF_Token *)lf_alloc_list_elem(tokens);
      token->type = cur_token_type;
      token->start_pos = src_pos;
      token->end_pos = src_pos;
      token->lexeme = NULL;

      continue;
    }

    _lf_push_lexeme_char(&cur_lexeme, cur_char);
  }

  bool has_remaining_lexeme = cur_lexeme.str_qtt >= 1;
  if (has_remaining_lexeme) {
    _lf_flush_lexeme(tokens, lexemes, &cur_lexeme, size);
  }

  // TODO: Dealloc string.

  LF_Token *eof_token = (LF_Token *)lf_alloc_list_elem(tokens);
  eof_token->type = LF_TOKEN_EOF;
  eof_token->start_pos = size;
  eof_token->end_pos = size;
  eof_token->lexeme = NULL;
}
