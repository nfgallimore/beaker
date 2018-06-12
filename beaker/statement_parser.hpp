#pragma once

#include <beaker/parser.hpp>

#include <queue>
#include <vector>

namespace beaker
{
  /// Parses a statement or sequence of statements.
  class Statement_parser : public Parser
  {
  public:
    Statement_parser(Parse_context& cxt)
      : Parser(cxt)
    { }

    Statement* parse_statement();
    Statement* parse_block_statement();
    Statement* parse_if_statement();
    Statement* parse_while_statement();
    Statement* parse_break_statement();
    Statement* parse_continue_statement();
    Statement* parse_return_statement();
    Statement* parse_declaration_statement();
    Statement* parse_expression_statement();
    Statement_seq parse_statement_seq();

    // Conditions
    Condition* parse_condition();

    // Local declarations.
    Declaration* parse_local_declaration();
    Declaration* parse_data_definition();

    // Assertions
    Declaration* parse_assertion();

    // Expressions
    Expression* parse_expression();
  };

} // namespace beaker
