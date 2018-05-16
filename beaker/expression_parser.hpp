#pragma once

#include <beaker/parser.hpp>

#include <deque>
#include <vector>

namespace beaker
{
  /// Parse expressions.
  class Expression_parser : public Parser
  {
  public:
    Expression_parser(Parse_context& cxt)
      : Parser(cxt)
    { }

    Token match_if_equality_operator();
    Token match_if_relational_operator();
    Token match_if_shift_operator();
    Token match_if_additive_operator();
    Token match_if_multiplicative_operator();
    Token match_if_unary_operator();

    Expression* parse_expression();
    Expression* parse_assignment_expression();
    Expression* parse_conditional_expression();
    Expression* parse_logical_or_expression();
    Expression* parse_logical_and_expression();
    Expression* parse_bitwise_or_expression();
    Expression* parse_bitwise_xor_expression();
    Expression* parse_bitwise_and_expression();
    Expression* parse_equality_expression();
    Expression* parse_relational_expression();
    Expression* parse_shift_expression();
    Expression* parse_additive_expression();
    Expression* parse_multiplicative_expression();
    Expression* parse_cast_expression();
    Expression* parse_unary_expression();
    Expression* parse_postfix_expression();
    Expression* parse_call_expression(Expression* e);
    Expression_seq parse_argument_list();
    Expression* parse_argument();
    Expression* parse_subscript_expression(Expression* e);
    Expression_seq parse_subscript_list();
    Expression* parse_subscript();
    Expression* parse_primary_expression();
    Expression* parse_id_expression();
  };

} // namespace beaker
