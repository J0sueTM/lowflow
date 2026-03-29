#include "./syntax_analysis.h"

#include "lexical_analysis.h"

void lf_parse_string(LF_Value *val, LF_Token *token) {
  assert(val);
  assert(token);
  assert(token->type == LF_TOKEN_STR);

  val->type = LF_STR;
  val->as_str = token->lexeme;
}

void lf_parse_expr(LF_List *vals, LF_List *tokens, LF_Token *cur_token) {
  assert(vals);
  assert(tokens);
  assert(cur_token);

  LF_Value *new_val = (LF_Value *)lf_alloc_list_elem(vals);
  switch (cur_token->type) {
    case LF_TOKEN_STR:
      lf_parse_string(new_val, cur_token);
      break;
    default:
      assert(false && "invalid token during expression parsing");
  }
}

// void lf_parse_arg(LF_Value *func_def, LF_List *arg_name_ptrs, LF_List
// *arg_types, LF_List *tokens, LF_Token *cur_token) {
//   assert(func_def);
//   assert(func_def->type.primitive_type == LF_FUNC_DEF);
//   assert(arg_name_ptrs);
//   assert(arg_types);
//   assert(tokens);
//   assert(cur_token);
//   assert(cur_token->type == LF_TOKEN_IDENTIFIER);
//   assert(func_def->func_def_spec);

//   LF_Token *comma_token = (LF_Token *)lf_get_next_list_elem(tokens, (char
//   *)cur_token); assert(comma_token); assert(comma_token->type ==
//   LF_TOKEN_COMMA && "invalid token during arg parsing: expects COMMA after
//   IDENTIFIER");

//   LF_Token *type_token = (LF_Token *)lf_get_next_list_elem(tokens, (char
//   *)comma_token); assert(type_token); assert(type_token->type ==
//   LF_TOKEN_IDENTIFIER && "invalid token during arg parsing: expects
//   IDENTIFIER after COMMA");

//   LF_Type *arg_type = (LF_Type *)lf_alloc_list_elem(arg_types);
//   arg_type->identifier = type_token->lexeme;

//   char **arg_name_ptr = (char **)lf_alloc_list_elem(arg_name_ptrs);
//   *arg_name_ptr = cur_token->lexeme;
//   // arg_qtt needs to be initialized before, so junk memory doesn't
//   // raise invalid arg qtts.
//   ++func_def->func_def_spec->arg_qtt;
// }
