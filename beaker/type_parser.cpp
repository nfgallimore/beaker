#include "type_parser.hpp"

#include <iostream>

namespace beaker
{
  /// type-specifier:
  ///   postfix-type-specifier
  Type_specifier*
  Type_parser::parse_type_specifier()
  {
    return parse_simple_type_specifier();
  }

  /// simple-type-specifier:
  ///   'unit' 
  ///   'bool' 
  ///   'int'   | 'int8'    | 'int16'   | 'int32'   | 'int64'    | 'int128'
  ///   'bin'   | 'bin8'    | 'bin16'   | 'bin32'   | 'bin64'    | 'bin128'
  ///   'float' | 'float16' | 'float32' | 'float64' | 'float128'
  ///   'char'  | 'char8'   | 'char16'  | 'char32'
  ///   '(' type-specifier-list ')' '->' type-specifier
  ///   '(' type-specifier ')'
  ///
  /// type-specifier-list:
  ///   type-specifier-list ',' type-specifier
  ///   type-specifier
  Type_specifier*
  Type_parser::parse_simple_type_specifier()
  {
    // FIXME: Handle precise types.
    switch (lookahead()) {
    case Token::unit_kw:
      return m_act.on_unit_type(consume());
    case Token::bool_kw:
      return m_act.on_bool_type(consume());
    case Token::int_kw:
      return m_act.on_int_type(consume());
    case Token::float_kw:
      return m_act.on_float_type(consume());
    case Token::char_kw:
      return m_act.on_char_type(consume());

    case Token::lparen: {
      // FIXME: Handle function types. Parse this as a type-specifier-list 
      // and determine the meaning later.
      Token lparen = match(Token::lparen);
      Type_specifier* ts = parse_type_specifier();
      Token rparen = match(Token::rparen);
      return ts;
    }

    default:
      throw std::runtime_error("expected basic-type");
    }
  }

} // namespace beaker
