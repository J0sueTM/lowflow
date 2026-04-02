#include <stdalign.h>

#include "../../src/frontend/lexical_analysis.h"
#include "../../vendor/munit/munit.h"

static MunitResult test_lex_single_char_tokens(const MunitParameter params[],
                                               void *data) {
  (void)params;
  (void)data;

  LF_Arena src_arena = {.block_offset = LF_MEMBLOCK_OFFSET_MIDDLE};
  LF_String src = {.char_qtt_in_block = 11, .arena = &src_arena};
  lf_init_string(&src);
  char *src_cstr = lf_alloc_string(&src, 11);
  strncpy(src_cstr, "=!/(){} = !", 11);

  LF_List tokens = {
    .elem_size = sizeof(LF_Token),
    .elem_alignment = alignof(LF_Token),
    .elem_qtt_in_block = 9,
  };
  lf_init_list(&tokens);

  LF_List lexemes = {
    .elem_size = sizeof(LF_String),
    .elem_alignment = alignof(LF_String),
    .elem_qtt_in_block = 9,
  };
  lf_init_list(&lexemes);

  LF_List lexeme_slices = {
    .elem_size = sizeof(LF_ArenaSlice),
    .elem_alignment = sizeof(LF_ArenaSlice),
    .elem_qtt_in_block = 9,
  };
  lf_init_list(&lexeme_slices);

  lf_lex(&tokens, &lexemes, &lexeme_slices, &src);

  munit_assert_size(tokens.elem_count, ==, 9);
  LF_TokenType expected_token_types[] = {
    LF_TOKEN_IDENTIFIER,
    LF_TOKEN_SLASH,
    LF_TOKEN_PAREN_BEG,
    LF_TOKEN_PAREN_END,
    LF_TOKEN_BRACE_BEG,
    LF_TOKEN_BRACE_END,
    LF_TOKEN_EQUAL,
    LF_TOKEN_BANG,
    LF_TOKEN_EOF,
  };
  LF_Token *cur_token = (LF_Token *)lf_get_first_list_elem(&tokens);

  LF_StringComparison comp = {
    .type = LF_STR_COMPARISON_STR_CHAR,
    .left = cur_token->lexeme,
    .right_char = "=!",
    .right_char_size = 2,
  };
  bool str_equal = lf_compare_string(&comp);
  munit_assert_int(str_equal, !=, 0);

  for (int i = 0; i < 7; ++i) {
    munit_assert_int(cur_token->type, ==, expected_token_types[i]);
    cur_token = (LF_Token *)lf_get_next_list_elem(&tokens, (char *)cur_token);
  }

  return MUNIT_OK;
}

static MunitResult test_lex_keywords(const MunitParameter params[],
                                     void *data) {
  (void)params;
  (void)data;

  LF_Arena src_arena = {.block_offset = LF_MEMBLOCK_OFFSET_MIDDLE};
  LF_String src = {.char_qtt_in_block = 14, .arena = &src_arena};
  lf_init_string(&src);
  char *src_cstr = lf_alloc_string(&src, 14);
  strncpy(src_cstr, "use entry func", 14);

  LF_List tokens = {
    .elem_size = sizeof(LF_Token),
    .elem_alignment = alignof(LF_Token),
    .elem_qtt_in_block = 4,
  };
  lf_init_list(&tokens);

  LF_List lexemes = {
    .elem_size = sizeof(LF_String),
    .elem_alignment = alignof(LF_String),
    .elem_qtt_in_block = 4,
  };
  lf_init_list(&lexemes);

  LF_List lexeme_slices = {
    .elem_size = sizeof(LF_ArenaSlice),
    .elem_alignment = sizeof(LF_ArenaSlice),
    .elem_qtt_in_block = 4,
  };
  lf_init_list(&lexeme_slices);

  lf_lex(&tokens, &lexemes, &lexeme_slices, &src);

  munit_assert_size(tokens.elem_count, ==, 4);

  LF_TokenType expected_token_types[] = {
    LF_TOKEN_USE,
    LF_TOKEN_ENTRY,
    LF_TOKEN_FUNC,
    LF_TOKEN_EOF,
  };
  LF_Token *cur_token = (LF_Token *)lf_get_first_list_elem(&tokens);
  for (int i = 0; i < 4; ++i) {
    munit_assert_int(cur_token->type, ==, expected_token_types[i]);
    cur_token = (LF_Token *)lf_get_next_list_elem(&tokens, (char *)cur_token);
  }

  return MUNIT_OK;
}

static MunitResult test_lex_identifier(const MunitParameter params[],
                                       void *data) {
  (void)params;
  (void)data;

  LF_Arena src_arena = {.block_offset = LF_MEMBLOCK_OFFSET_MIDDLE};
  LF_String src = {.char_qtt_in_block = 6, .arena = &src_arena};
  lf_init_string(&src);
  char *src_cstr = lf_alloc_string(&src, 6);
  strncpy(src_cstr, "my_var", 6);

  LF_List tokens = {
    .elem_size = sizeof(LF_Token),
    .elem_alignment = alignof(LF_Token),
    .elem_qtt_in_block = 2,
  };
  lf_init_list(&tokens);

  LF_List lexemes = {
    .elem_size = sizeof(LF_String),
    .elem_alignment = alignof(LF_String),
    .elem_qtt_in_block = 1,
  };
  lf_init_list(&lexemes);

  LF_List lexeme_slices = {
    .elem_size = sizeof(LF_ArenaSlice),
    .elem_alignment = sizeof(LF_ArenaSlice),
    .elem_qtt_in_block = 1,
  };
  lf_init_list(&lexeme_slices);

  lf_lex(&tokens, &lexemes, &lexeme_slices, &src);

  munit_assert_size(tokens.elem_count, ==, 2);

  LF_Token *fst_token = (LF_Token *)lf_get_first_list_elem(&tokens);
  munit_assert_int(fst_token->type, ==, LF_TOKEN_IDENTIFIER);

  LF_StringComparison comp = {.type = LF_STR_COMPARISON_STR_CHAR,
                              .left = fst_token->lexeme,
                              .right_char = "my_var",
                              .right_char_size = 6};
  bool str_equal = lf_compare_string(&comp);
  munit_assert_int(str_equal, !=, 0);

  LF_Token *snd_token =
    (LF_Token *)lf_get_next_list_elem(&tokens, (char *)fst_token);
  munit_assert_int(snd_token->type, ==, LF_TOKEN_EOF);

  return MUNIT_OK;
}

static MunitResult test_lex_big_lexemes(const MunitParameter params[],
                                        void *data) {
  (void)params;
  (void)data;

  LF_Arena src_arena = {.block_offset = LF_MEMBLOCK_OFFSET_MIDDLE};
  LF_String src = {.char_qtt_in_block = 13, .arena = &src_arena};
  lf_init_string(&src);
  char *src_cstr = lf_alloc_string(&src, 13);
  strncpy(src_cstr, "= =identifier", 13);

  LF_List tokens = {
    .elem_size = sizeof(LF_Token),
    .elem_alignment = alignof(LF_Token),
    .elem_qtt_in_block = 3,
  };
  lf_init_list(&tokens);

  LF_List lexemes = {
    .elem_size = sizeof(LF_String),
    .elem_alignment = alignof(LF_String),
    .elem_qtt_in_block = 2,
  };
  lf_init_list(&lexemes);

  LF_List lexeme_slices = {
    .elem_size = sizeof(LF_ArenaSlice),
    .elem_alignment = sizeof(LF_ArenaSlice),
    .elem_qtt_in_block = 1,
  };
  lf_init_list(&lexeme_slices);

  lf_lex(&tokens, &lexemes, &lexeme_slices, &src);

  munit_assert_size(tokens.elem_count, ==, 3);

  LF_Token *fst_token = (LF_Token *)lf_get_first_list_elem(&tokens);
  munit_assert_int(fst_token->type, ==, LF_TOKEN_EQUAL);

  LF_Token *snd_token =
    (LF_Token *)lf_get_next_list_elem(&tokens, (char *)fst_token);
  munit_assert_int(snd_token->type, ==, LF_TOKEN_IDENTIFIER);

  LF_StringComparison comp = {
    .type = LF_STR_COMPARISON_STR_CHAR,
    .left = snd_token->lexeme,
    .right_char = "=identifier",
    .right_char_size = 11,
  };
  bool str_equal = lf_compare_string(&comp);
  munit_assert_size(str_equal, !=, 0);

  LF_Token *thd_token =
    (LF_Token *)lf_get_next_list_elem(&tokens, (char *)snd_token);
  munit_assert_int(thd_token->type, ==, LF_TOKEN_EOF);

  return MUNIT_OK;
}

static MunitResult test_lex_whitespace(const MunitParameter params[],
                                       void *data) {
  (void)params;
  (void)data;

  LF_Arena src_arena = {.block_offset = LF_MEMBLOCK_OFFSET_MIDDLE};
  LF_String src = {.char_qtt_in_block = 17, .arena = &src_arena};
  lf_init_string(&src);
  char *src_cstr = lf_alloc_string(&src, 17);
  strncpy(src_cstr, "  use  \n\t entry  ", 17);

  LF_List tokens = {
    .elem_size = sizeof(LF_Token),
    .elem_alignment = alignof(LF_Token),
    .elem_qtt_in_block = 2,
  };
  lf_init_list(&tokens);

  LF_List lexemes = {
    .elem_size = sizeof(LF_String),
    .elem_alignment = alignof(LF_String),
    .elem_qtt_in_block = 2,
  };
  lf_init_list(&lexemes);

  LF_List lexeme_slices = {
    .elem_size = sizeof(LF_ArenaSlice),
    .elem_alignment = sizeof(LF_ArenaSlice),
    .elem_qtt_in_block = 2,
  };
  lf_init_list(&lexeme_slices);

  lf_lex(&tokens, &lexemes, &lexeme_slices, &src);

  munit_assert_size(tokens.elem_count, ==, 3);

  LF_TokenType expected_token_types[] = {
    LF_TOKEN_USE,
    LF_TOKEN_ENTRY,
    LF_TOKEN_EOF,
  };
  LF_Token *cur_token = (LF_Token *)lf_get_first_list_elem(&tokens);
  for (int i = 0; i < 3; ++i) {
    munit_assert_int(cur_token->type, ==, expected_token_types[i]);
    cur_token = (LF_Token *)lf_get_next_list_elem(&tokens, (char *)cur_token);
  }

  return MUNIT_OK;
}

static MunitResult test_lex_mixed_tokens(const MunitParameter params[],
                                         void *data) {
  (void)params;
  (void)data;

  LF_Arena src_arena = {.block_offset = LF_MEMBLOCK_OFFSET_MIDDLE};
  LF_String src = {.char_qtt_in_block = 47, .arena = &src_arena};
  lf_init_string(&src);
  char *src_cstr = lf_alloc_string(&src, 47);
  strncpy(src_cstr, "func hello-world: () Str = {\n  \"hello world!\"\n}", 47);

  LF_List tokens = {
    .elem_size = sizeof(LF_Token),
    .elem_alignment = alignof(LF_Token),
    .elem_qtt_in_block = 10,
  };
  lf_init_list(&tokens);

  LF_List lexemes = {
    .elem_size = sizeof(LF_String),
    .elem_alignment = alignof(LF_String),
    .elem_qtt_in_block = 3,
  };
  lf_init_list(&lexemes);

  LF_List lexeme_slices = {
    .elem_size = sizeof(LF_ArenaSlice),
    .elem_alignment = sizeof(LF_ArenaSlice),
    .elem_qtt_in_block = 3,
  };
  lf_init_list(&lexeme_slices);

  lf_lex(&tokens, &lexemes, &lexeme_slices, &src);

  munit_assert_size(tokens.elem_count, ==, 11);

  LF_TokenType expected_token_types[] = {
    LF_TOKEN_FUNC,
    LF_TOKEN_IDENTIFIER,
    LF_TOKEN_COLON,
    LF_TOKEN_PAREN_BEG,
    LF_TOKEN_PAREN_END,
    LF_TOKEN_IDENTIFIER,
    LF_TOKEN_EQUAL,
    LF_TOKEN_BRACE_BEG,
    LF_TOKEN_STR,
    LF_TOKEN_BRACE_END,
    LF_TOKEN_EOF,
  };
  LF_Token *cur_token = (LF_Token *)lf_get_first_list_elem(&tokens);
  for (int i = 0; i < 11; ++i) {
    munit_assert_int(cur_token->type, ==, expected_token_types[i]);
    cur_token = (LF_Token *)lf_get_next_list_elem(&tokens, (char *)cur_token);
  }

  return MUNIT_OK;
}

static MunitTest lexical_analysis_tests[] = {
  {
    .name = "/single_char_tokens",
    .test = test_lex_single_char_tokens,
    .setup = NULL,
    .tear_down = NULL,
    .options = MUNIT_TEST_OPTION_NONE,
    .parameters = NULL,
  },
  {
    .name = "/keywords",
    .test = test_lex_keywords,
    .setup = NULL,
    .tear_down = NULL,
    .options = MUNIT_TEST_OPTION_NONE,
    .parameters = NULL,
  },
  {
    .name = "/identifier",
    .test = test_lex_identifier,
    .setup = NULL,
    .tear_down = NULL,
    .options = MUNIT_TEST_OPTION_NONE,
    .parameters = NULL,
  },
  {
    .name = "/big_lexemes",
    .test = test_lex_big_lexemes,
    .setup = NULL,
    .tear_down = NULL,
    .options = MUNIT_TEST_OPTION_NONE,
    .parameters = NULL,
  },
  {
    .name = "/whitespace",
    .test = test_lex_whitespace,
    .setup = NULL,
    .tear_down = NULL,
    .options = MUNIT_TEST_OPTION_NONE,
    .parameters = NULL,
  },
  {
    .name = "/mixed_tokens",
    .test = test_lex_mixed_tokens,
    .setup = NULL,
    .tear_down = NULL,
    .options = MUNIT_TEST_OPTION_NONE,
    .parameters = NULL,
  },
  {0},
};

static const MunitSuite lexical_analysis_test_suite = {
  .prefix = "/lexical_analysis",
  .tests = lexical_analysis_tests,
  .suites = NULL,
  .iterations = 1,
  .options = MUNIT_SUITE_OPTION_NONE,
};
