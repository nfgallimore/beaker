#include "function_parser.hpp"
#include "type_parser.hpp"
#include "expression_parser.hpp"
#include "data_parser.hpp"
#include "dump.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace beaker
{
  /// function-signature:
  ///   '(' parameter-clause ')' '->' type-specifier
  ///
  /// \todo Allow the return type to be omitted?
  void
  Function_parser::parse_function_signature(Declaration* d)
  {
    // Re-enter the enclosing contex.t
    Restored_declarative_region region(*this, d);

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
  /// \todo Implement support for val and var parameters.
  ///
  /// \todo Allow for unnamed parameters.
  ///
  /// \todo Support default arguments.
  Parameter*
  Function_parser::parse_function_parameter()
  {
    Token id = require(Token::identifier);
    
    Token colon = match(Token::colon);
    Type_parser tp(m_cxt);
    Type_specifier* type = tp.parse_value_type_specifier();
    
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
    // Re-enter the enclosing context.
    Restored_declarative_region region(*this, d);

    // Re-enter function scope.
    //
    // FIXME: If we want function labels, then we really need something
    // between the statement scope and the declaration scope. Lables are
    // part of the definition, and shouldn't be registered with the function.
    //
    // Except perhaps that registering a label with a function might expose
    // an entry point for coroutines?
    Parsing_declarative_region function(*this, d);

    Statement* body = m_act.on_start_function_definition(d);
    Token lbrace = require(Token::lbrace);
    Statement_seq stmts;
    if (next_token_is_not(Token::rbrace))
      stmts = parse_statement_seq();
    Token rbrace = match(Token::rbrace);
    m_act.on_finish_function_definition(d, body, lbrace, rbrace);
  }

  /// statement-seq:
  ///   statement-seq statement
  ///   statement
  Statement_seq 
  Function_parser::parse_statement_seq()
  {
    Statement_seq ss;
    do {
      Statement* s = parse_statement();
      ss.push_back(s);
    } while (next_token_is_not(Token::rbrace));
    return ss;
  }

  /// statement:
  ///   block-statement
  ///   if-statement
  ///   while-statement
  ///   break-statement
  ///   continue-statement
  ///   return-statement
  ///   declaration-statement
  ///   expression-statement
  Statement*
  Function_parser::parse_statement()
  {
    switch (lookahead()) {
    case Token::lbrace:
      return parse_block_statement();

    case Token::if_kw:
      return parse_if_statement();
    
    case Token::while_kw:
      return parse_while_statement();
    
    case Token::break_kw:
      return parse_break_statement();
    
    case Token::continue_kw:
      return parse_continue_statement();
    
    case Token::return_kw:
      return parse_return_statement();

    case Token::var_kw:
    case Token::val_kw:
      return parse_declaration_statement();

    default:
      return parse_expression_statement();
    }
  }

  /// block-statement:
  ///   '{' statement-seq '}'
  Statement*
  Function_parser::parse_block_statement()
  {
    Token lbrace = match(Token::lbrace);
    Statement_seq stmts;
    if (lookahead() != Token::rbrace)
      stmts = parse_statement_seq();
    Token rbrace = match(Token::rbrace);
    return m_act.on_block_statement(stmts, lbrace, rbrace);
  }

  /// if-statement:
  ///   'if' '(' condition ')' statement
  ///   'if' '(' condition ')' statement 'else' statement
  Statement*
  Function_parser::parse_if_statement()
  {
    Token kw1 = require(Token::if_kw);
    Token lparen = match(Token::lparen);
    Expression* e = parse_expression();
    Token rparen = match(Token::rparen);
    Statement* s1 = parse_statement();
    if (Token kw2 = match(Token::else_kw)) {
      Statement* s2 = parse_statement();
      return m_act.on_if_statement(e, s1, s2, kw1, lparen, rparen, kw2);
    }
    else {
      return m_act.on_when_statement(e, s1, kw1, lparen, rparen);
    }
  }

  /// while-statement:
  ///   'while' '(' condition ')' statement
  Statement*
  Function_parser::parse_while_statement()
  {
    Token kw = require(Token::while_kw);
    Token lparen = match(Token::lparen);
    Expression* e = parse_expression();
    Token rparen = match(Token::rparen);
    Statement* s = parse_statement();
    return m_act.on_while_statement(e, s, kw, lparen, rparen);
  }

  /// break-statement:
  ///   'break' ';'
  Statement*
  Function_parser::parse_break_statement()
  {
    Token kw = require(Token::break_kw);
    Token semi = match(Token::semicolon);
    return m_act.on_break_statement(kw, semi);
  }

  /// continue-statement:
  ///   'continue' ';'
  Statement*
  Function_parser::parse_continue_statement()
  {
    Token kw = require(Token::continue_kw);
    Token semi = match(Token::semicolon);
    return m_act.on_continue_statement(kw, semi);
  }

  /// return-statement:
  ///   'return' expression ';'
  Statement*
  Function_parser::parse_return_statement()
  {
    Token kw = require(Token::return_kw);
    Expression* e = parse_expression();
    Token semi = match(Token::semicolon);
    return m_act.on_return_statement(e, kw, semi);
  }

  /// declaration-statement:
  ///   local-declaration
  Statement*
  Function_parser::parse_declaration_statement()
  {
    Declaration* d = parse_local_declaration();
    return m_act.on_declaration_statement(d);
  }

  /// expression-statement:
  ///   expression ';'
  Statement*
  Function_parser::parse_expression_statement()
  {
    Expression* e = parse_expression();
    Token semi = match(Token::semicolon);
    return m_act.on_expression_statement(e, semi);
  }

  /// condition:
  ///   expression
  ///
  /// \todo Allow declarations within the condition?
  Condition*
  Function_parser::parse_condition()
  {
    Expression* expr = parse_expression();
    return m_act.on_condition(expr);
  }


  /// local-declaration:
  ///   data-definition
  Declaration*
  Function_parser::parse_local_declaration()
  {
    switch (lookahead()) {
    case Token::val_kw:
    case Token::var_kw:
      return parse_data_definition();
    default:
      break;
    }
    throw std::runtime_error("expected local-declaration");
  }

  Declaration*
  Function_parser::parse_data_definition()
  {
    assert(next_token_is(Token::val_kw) || next_token_is(Token::var_kw));
    Token kw = consume();

    // Match the declaration name.
    Token id = match(Token::identifier);

    // Point of identification.
    Declaration* data = m_act.on_data_identification(id, kw);

    Data_parser dp(m_cxt);
    dp.parse_data_type(data);
    dp.parse_data_initializer(data);

    return data;    
  }

  /// Parses an expression.
  Expression*
  Function_parser::parse_expression()
  {
    Expression_parser ep(m_cxt);
    return ep.parse_expression();
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