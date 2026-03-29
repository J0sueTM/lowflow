#include <stdalign.h>

#include "../../src/frontend/lexical_analysis.h"
#include "../../vendor/munit/munit.h"

static MunitResult test_lex_single_char_tokens(const MunitParameter params[],
                                               void *data) {
  (void)params;
  (void)data;

  char *source = "=!/(){} = !";
  LF_List tokens = {.elem_size = sizeof(LF_Token),
                    .elem_alignment = alignof(LF_Token),
                    .elem_qtt_in_block = 8};
  lf_init_list(&tokens);
  LF_String lexemes = {.char_qtt_in_block = 64};
  lf_init_string(&lexemes);

  lf_lex(&tokens, &lexemes, source, 11);

  munit_assert_size(tokens.elem_count, ==, 9);
  LF_TokenType expected_token_types[] = {LF_TOKEN_IDENTIFIER,
                                         LF_TOKEN_SLASH,
                                         LF_TOKEN_PAREN_BEG,
                                         LF_TOKEN_PAREN_END,
                                         LF_TOKEN_BRACE_BEG,
                                         LF_TOKEN_BRACE_END,
                                         LF_TOKEN_EQUAL,
                                         LF_TOKEN_BANG,
                                         LF_TOKEN_EOF};
  LF_Token *cur_token = (LF_Token *)lf_get_first_list_elem(&tokens);
  munit_assert_string_equal(cur_token->lexeme, "=!");
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

  char *source = "use entry func";
  LF_List tokens = {.elem_size = sizeof(LF_Token),
                    .elem_alignment = alignof(LF_Token),
                    .elem_qtt_in_block = 8};
  lf_init_list(&tokens);
  LF_String lexemes = {.char_qtt_in_block = 64};
  lf_init_string(&lexemes);

  lf_lex(&tokens, &lexemes, source, 14);

  munit_assert_size(tokens.elem_count, ==, 4);

  LF_TokenType expected_token_types[] = {LF_TOKEN_USE,
                                         LF_TOKEN_ENTRY,
                                         LF_TOKEN_FUNC,
                                         LF_TOKEN_EOF};
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

  char *source = "my_var";
  LF_List tokens = {.elem_size = sizeof(LF_Token),
                    .elem_alignment = alignof(LF_Token),
                    .elem_qtt_in_block = 8};
  lf_init_list(&tokens);
  LF_String lexemes = {.char_qtt_in_block = 64};
  lf_init_string(&lexemes);

  lf_lex(&tokens, &lexemes, source, 6);

  munit_assert_size(tokens.elem_count, ==, 2);

  LF_Token *fst_token = (LF_Token *)lf_get_first_list_elem(&tokens);
  munit_assert_int(fst_token->type, ==, LF_TOKEN_IDENTIFIER);
  munit_assert_string_equal(fst_token->lexeme, "my_var");

  LF_Token *snd_token =
    (LF_Token *)lf_get_next_list_elem(&tokens, (char *)fst_token);
  munit_assert_int(snd_token->type, ==, LF_TOKEN_EOF);

  return MUNIT_OK;
}

static MunitResult test_lex_big_lexemes(const MunitParameter params[],
                                        void *data) {
  (void)params;
  (void)data;

  char *source = "= =identifier";
  LF_List tokens = {.elem_size = sizeof(LF_Token),
                    .elem_alignment = alignof(LF_Token),
                    .elem_qtt_in_block = 8};
  lf_init_list(&tokens);
  LF_String lexemes = {.char_qtt_in_block = 64};
  lf_init_string(&lexemes);

  lf_lex(&tokens, &lexemes, source, 15);

  munit_assert_size(tokens.elem_count, ==, 3);

  LF_Token *fst_token = (LF_Token *)lf_get_first_list_elem(&tokens);
  munit_assert_int(fst_token->type, ==, LF_TOKEN_EQUAL);

  LF_Token *snd_token =
    (LF_Token *)lf_get_next_list_elem(&tokens, (char *)fst_token);
  munit_assert_int(snd_token->type, ==, LF_TOKEN_IDENTIFIER);
  munit_assert_string_equal(snd_token->lexeme, "=identifier");

  LF_Token *thd_token =
    (LF_Token *)lf_get_next_list_elem(&tokens, (char *)snd_token);
  munit_assert_int(thd_token->type, ==, LF_TOKEN_EOF);

  return MUNIT_OK;
}

static MunitResult test_lex_whitespace_handling(const MunitParameter params[],
                                                void *data) {
  (void)params;
  (void)data;

  char *source = "  use  \n\t entry  ";
  LF_List tokens = {.elem_size = sizeof(LF_Token),
                    .elem_alignment = alignof(LF_Token),
                    .elem_qtt_in_block = 8};
  lf_init_list(&tokens);
  LF_String lexemes = {.char_qtt_in_block = 64};
  lf_init_string(&lexemes);

  lf_lex(&tokens, &lexemes, source, 17);

  munit_assert_size(tokens.elem_count, ==, 3);

  LF_TokenType expected_token_types[] = {LF_TOKEN_USE,
                                         LF_TOKEN_ENTRY,
                                         LF_TOKEN_EOF};
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

  char *source = "func hello-world: () Str = {\n  \"hello world!\"\n}";
  LF_List tokens = {.elem_size = sizeof(LF_Token),
                    .elem_alignment = alignof(LF_Token),
                    .elem_qtt_in_block = 8};
  lf_init_list(&tokens);
  LF_String lexemes = {.char_qtt_in_block = 64};
  lf_init_string(&lexemes);

  lf_lex(&tokens, &lexemes, source, 47);

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
    .name = "/whitespace_handling",
    .test = test_lex_whitespace_handling,
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
  {0}};

static const MunitSuite lexical_analysis_test_suite = {
  .prefix = "/lexical_analysis",
  .tests = lexical_analysis_tests,
  .suites = NULL,
  .iterations = 1,
  .options = MUNIT_SUITE_OPTION_NONE,
};
