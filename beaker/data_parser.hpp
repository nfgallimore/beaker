#pragma once

#include <beaker/parser.hpp>

#include <queue>
#include <vector>

namespace beaker
{
  /// Parses a datum definition.
  class Data_parser : public Parser
  {
  public:
    Data_parser(Parse_context& cxt)
      : Parser(cxt)
    { }

    void parse_data_type(Declaration* d);
    void parse_data_initializer(Declaration* d);

    // Expressions
    Expression* parse_expression();

    // Types
    Type_specifier* parse_type_specifier();
  };

} // namespace beaker
