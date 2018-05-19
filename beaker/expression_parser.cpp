#include "expression_parser.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace beaker
{
  /// Matches if the current token is == or !=.
  Token
  Expression_parser::match_if_equality_operator()
  {
    switch (lookahead()) {
    case Token::equal_equal:
    case Token::bang_equal:
      return consume();
    default:
      return {};
    }
  }

  /// Matches if the current token is <, >, <=, or >=.
  Token
  Expression_parser::match_if_relational_operator()
  {
    switch (lookahead()) {
    case Token::less:
    case Token::greater:
    case Token::less_equal:
    case Token::greater_equal:
      return consume();
    default:
      return {};
    }
  }

  /// Matches if the current token is << or >>.
  Token
  Expression_parser::match_if_shift_operator()
  {
    switch (lookahead()) {
    case Token::less_less:
    case Token::greater_greater:
      return consume();
    default:
      return {};
    }
  }

  /// Matches if the current token is + or -.
  Token
  Expression_parser::match_if_additive_operator()
  {
    switch (lookahead()) {
    case Token::plus:
    case Token::minus:
      return consume();
    default:
      return {};
    }
  }

  /// Matches if the current token is *, /, %.
  Token
  Expression_parser::match_if_multiplicative_operator()
  {
    switch (lookahead()) {
    case Token::star:
    case Token::slash:
    case Token::percent:
      return consume();
    default:
      return {};
    }
  }

  /// unary-operator:
  ///   '+' | '-' | '~' | '!'
  Token
  Expression_parser::match_if_unary_operator()
  {
    switch (lookahead()) {
    case Token::plus:
    case Token::minus:
    case Token::tilde:
    case Token::bang:
      return consume();
    default:
      return {};
    }
  }

  /// expression:
  ///   assignment-expression
  Expression*
  Expression_parser::parse_expression()
  {
    return parse_assignment_expression();
  }

  /// assignment-expression:
  ///   conditional-expression '=' assignment-expression
  ///   conditional-expression
  Expression*
  Expression_parser::parse_assignment_expression()
  {
    Expression* e1 = parse_conditional_expression();
    if (Token op = match_if(Token::equal)) {
      Expression* e2 = parse_assignment_expression();
      return m_act.on_assignment_expression(e1, e2, op);
    }
    return e1;
  }

  /// conditional-expression:
  ///   logical-or-expression '?' expression ':' conditional-expression
  ///   logical-or-expression
  Expression*
  Expression_parser::parse_conditional_expression()
  {
    Expression* e1 = parse_logical_or_expression();
    if (Token tok1 = match_if(Token::question)) {
      Expression* e2 = parse_expression();
      Token tok2 = match(Token::colon);
      Expression* e3 = parse_conditional_expression();
      return m_act.on_conditional_expression(e1, e2, e3, tok1, tok2);
    }
    return e1;
  }

  /// logical-or-expression:
  ///   logical-or-expression '||' logical-and-expression
  ///   logical-and-expression
  Expression*
  Expression_parser::parse_logical_or_expression()
  {
    Expression* e1 = parse_logical_and_expression();
    while (Token op = match_if(Token::bar_bar)) {
      Expression* e2 = parse_logical_and_expression();
      e1 = m_act.on_logical_expression(e1, e2, op);
    }
    return e1;
  }

  /// logical-and-expression:
  ///   logical-and-expression '&&' bitwise-or-expression
  ///   bitwise-or-expression
  Expression*
  Expression_parser::parse_logical_and_expression()
  {
    Expression* e1 = parse_bitwise_or_expression();
    while (Token op = match_if(Token::ampersand_ampersand)) {
      Expression* e2 = parse_bitwise_or_expression();
      e1 = m_act.on_logical_expression(e1, e2, op);
    }
    return e1;
  }

  /// bitwise-or-expression:
  ///   bitwise-or-expression '|' bitwise-xor-expression
  ///   bitwise-xor-expression
  Expression*
  Expression_parser::parse_bitwise_or_expression()
  {
    Expression* e1 = parse_bitwise_xor_expression();
    while (Token op = match_if(Token::bar)) {
      Expression* e2 = parse_bitwise_xor_expression();
      e1 = m_act.on_bitwise_expression(e1, e2, op);
    }
    return e1;
  }

  /// bitwise-xor-expression:
  ///   bitwise-xor-expression '^' bitwise-and-expression
  ///   bitwise-and-expression
  Expression*
  Expression_parser::parse_bitwise_xor_expression()
  {
    Expression* e1 = parse_bitwise_and_expression();
    while (Token op = match_if(Token::caret)) {
      Expression* e2 = parse_bitwise_and_expression();
      e1 = m_act.on_bitwise_expression(e1, e2, op);
    }
    return e1;
  }

  /// bitwise-and-expression:
  ///   bitwise-and-expression '&' equality-expression
  ///   equality-expression
  Expression*
  Expression_parser::parse_bitwise_and_expression()
  {
    Expression* e1 = parse_equality_expression();
    while (Token op = match_if(Token::ampersand)) {
      Expression* e2 = parse_equality_expression();
      e1 = m_act.on_bitwise_expression(e1, e2, op);
    }
    return e1;
  }

  /// equality-expression:
  ///   equality-expression '==' relational-expression
  ///   equality-expression '!=' relational-expression
  ///   relational-expression
  Expression*
  Expression_parser::parse_equality_expression()
  {
    Expression* e1 = parse_relational_expression();
    while (Token op = match_if_equality_operator()) {
      Expression* e2 = parse_relational_expression();
      e1 = m_act.on_equality_expression(e1, e2, op);
    }
    return e1;
  }

  /// relational-expression:
  ///   relational-expression '<' shift-expression
  ///   relational-expression '>' shift-expression
  ///   relational-expression '<=' shift-expression
  ///   relational-expression '>=' shift-expression
  ///   shift-expression
  Expression*
  Expression_parser::parse_relational_expression()
  {
    Expression* e1 = parse_shift_expression();
    while (Token op = match_if_relational_operator()) {
      Expression* e2 = parse_shift_expression();
      e1 = m_act.on_relational_expression(e1, e2, op);
    }
    return e1;
  }

  /// shift-expression:
  ///   shift-expression '<<' additive-expression
  ///   shift-expression '>>' additive-expression
  ///   additive-expression
  Expression*
  Expression_parser::parse_shift_expression()
  {
    Expression* e1 = parse_additive_expression();
    while (Token op = match_if_shift_operator()) {
      Expression* e2 = parse_additive_expression();
      e1 = m_act.on_shift_expression(e1, e2, op);
    }
    return e1;
  }

  /// additive-expression:
  ///   additive-expression '+' multiplicative-expression
  ///   additive-expression '-' multiplicative-expression
  ///   multiplicative-expression
  Expression*
  Expression_parser::parse_additive_expression()
  {
    Expression* e1 = parse_multiplicative_expression();
    while (Token op = match_if_additive_operator()) {
      Expression* e2 = parse_multiplicative_expression();
      e1 = m_act.on_additive_expression(e1, e2, op);
    }
    return e1;
  }

  /// multiplicative-expression:
  ///   multiplicative-expression '*' additive-expression
  ///   multiplicative-expression '/' additive-expression
  ///   multiplicative-expression '%' additive-expression
  ///   additive-expression
  Expression*
  Expression_parser::parse_multiplicative_expression()
  {
    Expression* e1 = parse_cast_expression();
    while (Token op = match_if_multiplicative_operator()) {
      Expression* e2 = parse_cast_expression();
      e1 = m_act.on_multiplicative_expression(e1, e2, op);
    }
    return e1;
  }

  Expression*
  Expression_parser::parse_cast_expression()
  {
    // Expression* e = parse_unary_expression();
    // if (match_if(kw_as)) {
    //   type* t = parse_type();
    //   return m_act.on_cast_expression(e, t);
    // }
    // return e;

    return parse_unary_expression();
  }

  /// unary-expression:
  ///   '+' unary-expression
  ///   '-' unary-expression
  ///   '~' unary-expression
  ///   '!' unary-expression
  ///   postfix-expression  
  Expression*
  Expression_parser::parse_unary_expression()
  {
    while (Token op = match_if_unary_operator()) {
      Expression* e = parse_unary_expression();
      return m_act.on_unary_expression(e, op);
    }
    return parse_postfix_expression();
  }

  /// postfix-expression:
  ///   postfix-expression '(' argument-list ')'
  ///   postfix-expression '[' subscript-list ']'
  ///   primary-expression
  Expression*
  Expression_parser::parse_postfix_expression()
  {
    Expression* e = parse_primary_expression();
    while (true) {
      if (next_token_is(Token::lparen))
        return parse_call_expression(e);
      else if (next_token_is(Token::lbracket))
        return parse_subscript_expression(e);
      else
        break;
    }
    return e;
  }

  /// postfix-expression:
  ///   postfix-expression '(' argument-list ')'
  Expression*
  Expression_parser::parse_call_expression(Expression* e)
  {
    Token lparen = require(Token::lparen);
    Expression_seq args = parse_argument_list();
    Token rparen = match(Token::rparen);
    return m_act.on_call_expression(e, args, lparen, rparen); 
  }

  /// argument-list:
  ///   argument-list ',' argument
  ///   argument
  Expression_seq
  Expression_parser::parse_argument_list()
  {
    Expression_seq args;
    while (true) {
      Expression* arg = parse_expression();
      if (match_if(Token::comma))
        continue;
      if (next_token_is(Token::rparen))
        break;
    }
    return args;
  }

  /// argument:
  ///   expression
  Expression*
  Expression_parser::parse_argument()
  {
    return parse_expression();
  }

  /// postfix-expression
  ///   postfix-expression '[' subscript-list ']'
  Expression*
  Expression_parser::parse_subscript_expression(Expression* e)
  {
    Token lbracket = require(Token::lbracket);
    Expression_seq args = parse_subscript_list();
    Token rbracket = match(Token::rbracket);
    return m_act.on_call_expression(e, args, lbracket, rbracket); 
  }

  /// subscript-list:
  ///   subscript-list ',' subscript
  ///   subscript
  Expression_seq
  Expression_parser::parse_subscript_list()
  {
    Expression_seq args;
    while (true) {
      Expression* arg = parse_expression();
      if (match_if(Token::comma))
        continue;
      if (next_token_is(Token::rbracket))
        break;
    }
    return args;
  }

  /// subscript:
  ///   expression
  Expression*
  Expression_parser::parse_subscript()
  {
    return parse_expression();
  }

  /// primary-expression:
  ///   literal
  ///   id-expression
  ///   '(' expression ')'
  Expression*
  Expression_parser::parse_primary_expression()
  {
    switch (lookahead()) {
    case Token::binary_integer:
    case Token::decimal_integer:
    case Token::hexadecimal_integer:
      return m_act.on_integer_literal(consume());
    
    case Token::unit_kw:
      return m_act.on_unit_literal(consume());

    case Token::true_kw:
    case Token::false_kw:
      return m_act.on_boolean_literal(consume());
    
    case Token::decimal_float:
    case Token::hexadecimal_float:
      return m_act.on_float_literal(consume());
    
    case Token::character:
      return m_act.on_character_literal(consume());

    case Token::string:
      return m_act.on_character_literal(consume());
    
    case Token::identifier:
      return parse_id_expression();
    
    case Token::lparen: {
      Token lparen = match(Token::lparen);
      Expression* e = parse_expression();
      Token rparen = match(Token::rparen);
      return m_act.on_paren_expression(e, lparen, rparen);
    }

    default:
      break;
    }
    
    throw std::runtime_error("expected primary-expression");
  }

  /// id-expression:
  ///   identifier
  ///
  /// \todo Allow unqualified and qualified ids, not just identifiers.
  Expression*
  Expression_parser::parse_id_expression()
  {
    Token id = require(Token::identifier);
    return m_act.on_id_expression(id);
  }

} // namespace beaker