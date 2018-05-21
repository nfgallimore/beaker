#include "data_parser.hpp"
#include "type_parser.hpp"
#include "expression_parser.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace beaker
{
  void
  Data_parser::parse_deferred_data_type(Declaration* d)
  {
    Restored_declarative_region region(*this, d);
    parse_data_type(d);
  }

  // type-clause:
  //    ':' value-type-specifier
  //    <empty>
  //
  // Note that the value-type-specifier does not permit reference types
  // in the declaration. To declare a reference, use a ref declaration.
  void
  Data_parser::parse_data_type(Declaration* d)
  {
    if (Token colon = match_if(Token::colon)) {
      Type_specifier* type = parse_value_type_specifier();
      m_act.on_data_declaration(d, type);
    }
    else {
      m_act.on_data_declaration(d);
    }
  }

  void
  Data_parser::parse_deferred_data_initializer(Declaration* d)
  {
    Restored_declarative_region region(*this, d);
    parse_data_initializer(d);
  }

  /// initializer:
  ///   '=' expression ';'
  ///   ';'
  ///
  /// \todo Support paren and brace initialization.
  void
  Data_parser::parse_data_initializer(Declaration* d)
  {
    if (Token eq = match_if(Token::equal)) {
      Expression* expr = parse_expression();
      Token semi = match(Token::semicolon);
      m_act.on_data_initialization(d, expr, semi);
    } else {
      Token semi = match(Token::semicolon);
      m_act.on_data_initialization(d, semi);
    }
  }

  /// Parses an expression.
  Expression*
  Data_parser::parse_expression()
  {
    Expression_parser ep(m_cxt);
    return ep.parse_expression();
  }

  /// Parse a value-type-specifier.
  Type_specifier*
  Data_parser::parse_value_type_specifier()
  {
    Type_parser tp(m_cxt);
    return tp.parse_value_type_specifier();
  }

} // namespace beaker