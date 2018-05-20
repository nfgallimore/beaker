#pragma once

#include <beaker/parser.hpp>

#include <deque>
#include <vector>

namespace beaker
{
  /// Parses type specifiers.
  class Type_parser : public Parser
  {
  public:
    Type_parser(Parse_context& cxt)
      : Parser(cxt)
    { }

    Type_specifier* parse_type_specifier();
    Type_specifier* parse_reference_type_specifier();
    Type_specifier* parse_value_type_specifier();
    Type_specifier* parse_simple_type_specifier();
    Type_specifier_seq parse_type_specifier_list();
  };

} // namespace beaker
