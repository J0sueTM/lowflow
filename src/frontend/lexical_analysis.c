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

static LF_TokenType _lf_get_token_type_from_big_lexeme(LF_String *str) {
  assert(str);

  LF_StringComparison comp = {
    .type = LF_STR_COMPARISON_STR_CHAR,
    .left = str
  };

  comp.right_char = "use";
  comp.right_char_size = 3;
  if (lf_compare_string(&comp)) {
    return LF_TOKEN_USE;
  }

  comp.right_char = "entry";
  comp.right_char_size = 5;
  if (lf_compare_string(&comp)) {
    return LF_TOKEN_ENTRY;
  }

  comp.right_char = "func";
  comp.right_char_size = 4;
  if (lf_compare_string(&comp)) {
    return LF_TOKEN_FUNC;
  }

  return LF_TOKEN_IDENTIFIER;
}

static void _lf_flush_lexeme(LF_List *tokens, LF_String *lexeme) {
  assert(tokens);
  assert(lexeme);

  LF_Token *token = (LF_Token *)lf_alloc_list_elem(tokens);
  if (lf_get_string_size(lexeme) <= 1) {
    LF_StringIteration lexeme_iter = {.str = lexeme};
    char *fst_char = lf_init_string_iteration(&lexeme_iter);
    LF_TokenType token_type = _lf_get_token_type_from_soft_boundary_lexeme(*fst_char);
    if (token_type == LF_TOKEN_UNKNOWN) {
      token_type = _lf_get_token_type_from_hard_boundary_lexeme(*fst_char);
    }
    token->type = token_type;
    token->lexeme = NULL;
  } else {
    token->type = _lf_get_token_type_from_big_lexeme(lexeme);
    if (token->type == LF_TOKEN_IDENTIFIER) {
      token->lexeme = lexeme;
    }
  }
}

void lf_lex(LF_List *tokens, LF_List *lexemes, LF_List *lexeme_slices, LF_String *src) {
  assert(tokens);
  assert(lexemes);
  assert(lexeme_slices);
  assert(src);

  LF_String *cur_lexeme = NULL;
  bool is_reading_str = false;

  LF_StringIteration src_iter = { .str = src };
  char *cur_char = lf_init_string_iteration(&src_iter);
  while (cur_char) {
    if (*cur_char == '"' && !is_reading_str) {
      LF_ArenaSlice *str_lexeme_slice = (LF_ArenaSlice *)lf_alloc_list_elem(lexeme_slices);
      str_lexeme_slice->head_block = src_iter.cur_block;
      str_lexeme_slice->tail_block = src_iter.cur_block;
      str_lexeme_slice->head_offset = src_iter.block_offset + sizeof(char);
      str_lexeme_slice->tail_offset = src_iter.block_offset + (sizeof(char) * 2);
      str_lexeme_slice->elem_size = sizeof(char);

      cur_lexeme = (LF_String *)lf_alloc_list_elem(lexemes);
      cur_lexeme->arena = src->arena;
      lf_init_string_from_slice(cur_lexeme, str_lexeme_slice);

      is_reading_str = true;
      continue;
    } else if (*cur_char == '"' && is_reading_str) {
      LF_Token *str_token = (LF_Token *)lf_alloc_list_elem(tokens);
      str_token->type = LF_TOKEN_STR;
      str_token->lexeme = cur_lexeme;

      cur_lexeme = NULL;
      is_reading_str = false;

      cur_char = lf_iterate_string(&src_iter);
      continue;
    } else if (is_reading_str) {
      lf_grow_arena_slice_tail(cur_lexeme->slice, cur_lexeme->arena, sizeof(char));

      cur_char = lf_iterate_string(&src_iter);
      continue;
    }

    bool has_old_lexeme = cur_lexeme != NULL;
    bool is_whitespace = _lf_is_whitespace(*cur_char);
    if (is_whitespace && has_old_lexeme) {
      _lf_flush_lexeme(tokens, cur_lexeme);

      cur_lexeme = NULL;
      cur_char = lf_iterate_string(&src_iter);
      continue;
    } else if (is_whitespace) {
      cur_char = lf_iterate_string(&src_iter);
      continue;
    }

    LF_TokenType hard_boundary_token_type = _lf_get_token_type_from_hard_boundary_lexeme(*cur_char);
    bool is_hard_boundary_token = hard_boundary_token_type != LF_TOKEN_UNKNOWN;
    if (is_hard_boundary_token) {
      if (has_old_lexeme) {
        _lf_flush_lexeme(tokens, cur_lexeme);
        cur_lexeme = NULL;
      }

      LF_Token *hard_boundary_token = (LF_Token *)lf_alloc_list_elem(tokens);
      hard_boundary_token->type = hard_boundary_token_type;

      cur_char = lf_iterate_string(&src_iter);
      continue;
    }

    bool has_new_lexeme = !has_old_lexeme;
    if (has_new_lexeme) {
      LF_ArenaSlice *cur_lexeme_slice = (LF_ArenaSlice *)lf_alloc_list_elem(lexeme_slices);
      cur_lexeme_slice->head_block = src_iter.cur_block;
      cur_lexeme_slice->tail_block = src_iter.cur_block;
      cur_lexeme_slice->head_offset = src_iter.block_offset;
      cur_lexeme_slice->tail_offset = src_iter.block_offset + sizeof(char);
      cur_lexeme_slice->elem_size = sizeof(char);

      cur_lexeme = (LF_String *)lf_alloc_list_elem(lexemes);
      cur_lexeme->arena = src->arena;
      lf_init_string_from_slice(cur_lexeme, cur_lexeme_slice);
    }

    if (has_old_lexeme) {
      lf_grow_arena_slice_tail(cur_lexeme->slice, cur_lexeme->arena, sizeof(char));
    }

    cur_char = lf_iterate_string(&src_iter);
  }

  bool has_remaining_lexeme = cur_lexeme != NULL;
  if (has_remaining_lexeme) {
    _lf_flush_lexeme(tokens, cur_lexeme);
    cur_lexeme = NULL;
  }

  LF_Token *eof_token = (LF_Token *)lf_alloc_list_elem(tokens);
  eof_token->type = LF_TOKEN_EOF;
  eof_token->lexeme = NULL;
}
