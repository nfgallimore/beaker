#include "statement_parser.hpp"
#include "expression_parser.hpp"
#include "data_parser.hpp"
#include "dump.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace beaker
{
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
  Statement_parser::parse_statement()
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
    case Token::ref_kw:
      return parse_declaration_statement();

    default:
      return parse_expression_statement();
    }
  }

  /// block-statement:
  ///   '{' statement-seq '}'
  Statement*
  Statement_parser::parse_block_statement()
  {
    Statement* s = m_act.on_start_block_statement();
    
    // Parse nested statements in block scope.
    Token lbrace, rbrace;
    {
      Parsing_declarative_region block(*this, s);
      lbrace = match(Token::lbrace);
      if (lookahead() != Token::rbrace)
        parse_statement_seq();
      rbrace = match(Token::rbrace);
    }
    
    return m_act.on_finish_block_statement(s, lbrace, rbrace);
  }

  /// if-statement:
  ///   'if' '(' condition ')' statement
  ///   'if' '(' condition ')' statement 'else' statement
  Statement*
  Statement_parser::parse_if_statement()
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
  Statement_parser::parse_while_statement()
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
  Statement_parser::parse_break_statement()
  {
    Token kw = require(Token::break_kw);
    Token semi = match(Token::semicolon);
    return m_act.on_break_statement(kw, semi);
  }

  /// continue-statement:
  ///   'continue' ';'
  Statement*
  Statement_parser::parse_continue_statement()
  {
    Token kw = require(Token::continue_kw);
    Token semi = match(Token::semicolon);
    return m_act.on_continue_statement(kw, semi);
  }

  /// return-statement:
  ///   'return' expression ';'
  Statement*
  Statement_parser::parse_return_statement()
  {
    Token kw = require(Token::return_kw);
    Expression* e = parse_expression();
    Token semi = match(Token::semicolon);
    return m_act.on_return_statement(e, kw, semi);
  }

  /// declaration-statement:
  ///   local-declaration
  Statement*
  Statement_parser::parse_declaration_statement()
  {
    Declaration* d = parse_local_declaration();
    return m_act.on_declaration_statement(d);
  }

  /// expression-statement:
  ///   expression ';'
  Statement*
  Statement_parser::parse_expression_statement()
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
  Statement_parser::parse_condition()
  {
    Expression* expr = parse_expression();
    return m_act.on_condition(expr);
  }


  /// local-declaration:
  ///   data-definition
  Declaration*
  Statement_parser::parse_local_declaration()
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

  /// FIXME: This is duplicated elsewhere.
  static bool 
  is_data_head(const Token& tok)
  {
    return tok.is(Token::val_kw)
        || tok.is(Token::var_kw)
        || tok.is(Token::ref_kw);
  }

  Declaration*
  Statement_parser::parse_data_definition()
  {
    assert(is_data_head(peek()));
    Token kw = consume();

    // Match the declaration name.
    Token id = match(Token::identifier);

    // Point of identification.
    Declaration* data = m_act.on_data_identification(id, kw);

    // Parse the type and initializer NOW.
    Data_parser dp(m_cxt);
    dp.parse_data_type(data);
    dp.parse_data_initializer(data);

    return data;
  }

  /// Parses an expression.
  Expression*
  Statement_parser::parse_expression()
  {
    Expression_parser ep(m_cxt);
    return ep.parse_expression();
  }

  /// statement-seq:
  ///   statement-seq statement
  ///   statement
  void
  Statement_parser::parse_statement_seq()
  {
    do {
      parse_statement();
    } while (next_token_is_not(Token::rbrace));
  }

} // namespace beaker