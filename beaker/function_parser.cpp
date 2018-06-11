#include "function_parser.hpp"
#include "type_parser.hpp"
#include "expression_parser.hpp"
#include "statement_parser.hpp"
#include "dump.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace beaker
{
  void
  Function_parser::parse_deferred_function_signature(Declaration* d)
  {
    Restored_declarative_region region(*this, d);
    parse_function_signature(d);
  }

  /// function-signature:
  ///   '(' parameter-clause ')' '->' type-specifier
  ///
  /// \todo Allow the return type to be omitted?
  void
  Function_parser::parse_function_signature(Declaration* d)
  {
    // Enter function scope.
    Parsing_declarative_region function(*this, d);

    // Match the parameter list.
    Token lparen = match(Token::lparen);
    Parameter_seq parms;
    if (lookahead() != Token::rparen)
      parms = parse_function_parameter_clause();
    Token rparen = match(Token::rparen);
    
    // FIXME: Make return type specifier option?
    Token arrow = match(Token::arrow);
    Type_parser tp(m_cxt);
    Type_specifier* type = tp.parse_type_specifier();

    // Point of declaration.
    m_act.on_function_declaration(d, std::move(parms), type, lparen, rparen, arrow);
  }

  /// function-parameter-clause:
  ///   function-parameter-list
  ///   function-parameter-list ',' ...
  Parameter_seq
  Function_parser::parse_function_parameter_clause()
  {
    Parameter_seq parms = parse_function_parameter_list();
    if (Token comma = match_if(Token::comma)) {
      if (next_token_is(Token::ellipsis)) {
        Parameter* parm = parse_variadic_parameter();
        parms.push_back(parm);
      }
    }
    return parms;
  }

  /// function-parameter-list:
  ///    function-parameter-list ',' function-parameter
  ///    function-parameter
  Parameter_seq
  Function_parser::parse_function_parameter_list()
  {
    Parameter_seq parms;
    while (true) {
      // Match the parameter.
      Parameter* parm = parse_function_parameter();
      parms.push_back(parm);
      
      // Stop on ', ...', but not on ','. We handle the variadic case in
      // the parameter-clause.
      if (next_token_is(Token::comma)) {
        if (nth_token_is(1, Token::ellipsis))
          break;
        consume();
        continue;
      }
      else {
        break;
      }
    }
    return parms;
  }

  /// function-parameter:
  ///   data-head identifier ':' value-type-specifier
  ///   identifier ':' value-type-specifier
  ///
  /// data-head:
  ///   'val' | 'var' | 'kind'
  ///
  /// \todo Allow for unnamed parameters.
  ///
  /// \todo Support default arguments.
  Parameter*
  Function_parser::parse_function_parameter()
  {
    Token kind;
    if (next_token_is(Token::val_kw, Token::var_kw, Token::ref_kw))
      kind = consume();

    Token id = require(Token::identifier);
    
    Token colon = match(Token::colon);
    Type_parser tp(m_cxt);
    Type_specifier* type = tp.parse_value_type_specifier();
    
    if (kind)
      return m_act.on_function_parameter(kind, id, type, colon);
    else
      return m_act.on_function_parameter(id, type, colon);
  }

  /// variadic-parameter:
  ///   '...'
  ///   '...' identifier
  ///
  /// \todo The type of `...args` is that the function type will be variadic.
  /// We use identifier to declare the argument list for use with various
  /// va_* operations. Note that this is NOT a generic function; it is provided
  /// only for support with C and C++.
  ///
  /// \todo Support variadic templates using the notation `f([]a : T)` where
  /// the `[]` denotes a list of arguments and `T` is its type or constraint.
  /// What should the expansion operator look like? Could be `...`.
  Parameter*
  Function_parser::parse_variadic_parameter()
  {
    Token ellipsis = require(Token::ellipsis);
    if (Token id = match_if(Token::identifier))
      return m_act.on_variadic_parameter(ellipsis, id);
    return m_act.on_variadic_parameter(ellipsis);
  }

  void
  Function_parser::parse_deferred_function_body(Declaration* d)
  {
    Restored_declarative_region region(*this, d);
    parse_function_body(d);
  }

  /// function-body:
  ///   '{' statement-seq '}'
  ///
  /// The function body is technically a compound statement, but there are
  /// semantic actions that we apply around the statement sequence that are
  /// not performed for normal compound statements.
  ///
  /// \todo Allow functions to be defined using '='?
  void
  Function_parser::parse_function_body(Declaration* d)
  {
    // Re-enter function scope.
    //
    // FIXME: If we want function labels, then we really need something
    // between the statement scope and the declaration scope. Lables are
    // part of the definition, and shouldn't be registered with the function.
    //
    // Except perhaps that registering a label with a function might expose
    // an entry point for coroutines?
    Parsing_declarative_region function(*this, d);

    m_act.on_start_function_definition(d);
    Statement_seq ss;
    Token lbrace = require(Token::lbrace);
    if (next_token_is_not(Token::rbrace))
      ss = parse_statement_seq();
    Token rbrace = match(Token::rbrace);
    m_act.on_finish_function_definition(d, std::move(ss), lbrace, rbrace);
  }

  /// statement-seq:
  ///   statement-seq statement
  ///   statement
  Statement_seq
  Function_parser::parse_statement_seq()
  {
    Statement_parser sp(m_cxt);
    return sp.parse_statement_seq();
  }

  /// Parse a type-specifier.
  Type_specifier*
  Function_parser::parse_type_specifier()
  {
    Type_parser tp(m_cxt);
    return tp.parse_type_specifier();
  }

  /// Parse a value-type-specifier.
  Type_specifier*
  Function_parser::parse_value_type_specifier()
  {
    Type_parser tp(m_cxt);
    return tp.parse_value_type_specifier();
  }

} // namespace beaker