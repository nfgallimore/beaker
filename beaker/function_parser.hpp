#pragma once

#include <beaker/parser.hpp>

#include <queue>
#include <vector>

namespace beaker
{
  /// Parses a function definition.
  class Function_parser : public Parser
  {
  public:
    Function_parser(Parse_context& cxt)
      : Parser(cxt)
    { }

    void parse_function_signature(Declaration* d);
    void parse_function_body(Declaration* d);

    // Function signature 
    Parameter_seq parse_function_parameter_clause();
    Parameter_seq parse_function_parameter_list();
    Parameter* parse_function_parameter();
    Parameter* parse_variadic_parameter();

    // Statements
    Statement_seq parse_statement_seq();
    Statement* parse_statement();
    Statement* parse_block_statement();
    Statement* parse_if_statement();
    Statement* parse_while_statement();
    Statement* parse_break_statement();
    Statement* parse_continue_statement();
    Statement* parse_return_statement();
    Statement* parse_declaration_statement();
    Statement* parse_expression_statement();

    // Conditions
    Condition* parse_condition();

    // Local declarations.
    Declaration* parse_local_declaration();
    Declaration* parse_data_definition();

    // Expressions
    Expression* parse_expression();

    // Types
    Type_specifier* parse_type_specifier();
  };

} // namespace beaker
