#ifndef LF_FRONTEND_SYNTAX_ANALYSIS_H
#define LF_FRONTEND_SYNTAX_ANALYSIS_H

#include "../core/memory/list.h"
#include "../core/types.h"
#include "./lexical_analysis.h"

void lf_parse_string(LF_Value *val, LF_Token *token);

void lf_parse_expr(LF_List *vals, LF_List *tokens, LF_Token *cur_token);

void lf_parse_arg(LF_Value *val, LF_List *tokens, LF_Token *cur_token);

// void lf_parse(LF_List *vals, LF_List *entrypoint_vals, LF_List *tokens);

#endif // LF_FRONTEND_SYNTAX_ANALYSIS_H
