#include "type_parser.hpp"

#include <iostream>
#include <sstream>

namespace beaker
{
  /// type-specifier:
  ///   reference-type-specifier
  Type_specifier*
  Type_parser::parse_type_specifier()
  {
    return parse_reference_type_specifier();
  }

  /// reference-type-specifier:
  ///   `ref` value-type-specifier
  ///   value-type-specifier
  Type_specifier*
  Type_parser::parse_reference_type_specifier()
  {
    Token ref = match_if(Token::ampersand);
    Type_specifier* ts = parse_value_type_specifier();
    if (ref)
      return m_act.on_reference_type(ts, ref);
    return ts;
  }

  /// value-type-specifier:
  ///   simple-type-specifier
  ///
  /// FIXME: Support syntax for pointers and arrays.
  Type_specifier*
  Type_parser::parse_value_type_specifier()
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
      // Match '(' type-specifier-list_opt ')'.
      //
      // FIXME: Handle a speciffier for variadic arguments.
      Token lparen = match(Token::lparen);
      Type_specifier_seq types;
      if (next_token_is_not(Token::rparen))
        types = parse_type_specifier_list();
      Token rparen = match(Token::rparen);

      /// Match '->' type-specifier.
      if (Token arrow = match_if(Token::arrow)) {
        Type_specifier* ret = parse_type_specifier();
        m_act.on_function_type(std::move(types), ret, lparen, rparen, arrow);
      }

      if (types.size() == 1)
        m_act.on_paren_type(types.front(), lparen, rparen);

      std::stringstream ss;
      ss << "unexpected type-specifier-list";
      throw std::runtime_error(ss.str());
    }

    default:
      throw std::runtime_error("expected basic-type");
    }
  }

  /// type-specifier-list:
  ///   type-specifier-list ',' type
  ///   
  Type_specifier_seq
  Type_parser::parse_type_specifier_list()
  {
    Type_specifier_seq specs;
    do {
      Type_specifier* ts = parse_type_specifier();
      specs.push_back(ts);
    } while (match_if(Token::comma));
    return specs;
  }

} // namespace beaker
