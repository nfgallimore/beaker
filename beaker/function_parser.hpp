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

    // Types
    Type_specifier* parse_type_specifier();
    Type_specifier* parse_value_type_specifier();
  };

} // namespace beaker
