#include "parser.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace beaker
{
  Parse_context::Parse_context(Context& cxt, const File& f)
    : m_lex(cxt, f), m_act(cxt), m_tok()
  {
    fetch();
  }

  Token::Name
  Parse_context::lookahead()
  {
    assert(!m_tok.empty());
    return m_tok.front().get_name();
  }

  Token::Name
  Parse_context::lookahead(int n)
  {
    if (n < m_tok.size())
      return m_tok[n].get_name();
    n = n - m_tok.size() + 1;
    while (n != 0) {
      fetch();
      --n;
    }
    return m_tok.back().get_name();
  }

  bool
  Parse_context::next_token_is(Token::Name k) 
  {
    return lookahead() == k;
  }

  bool
  Parse_context::next_token_is(Token::Name k1, Token::Name k2) 
  {
    return next_token_is(k1) || next_token_is(k2);
  }

  bool
  Parse_context::next_token_is(Token::Name k1, Token::Name k2, Token::Name k3) 
  {
    return next_token_is(k1, k2) || next_token_is(k3);
  }

  bool
  Parse_context::next_token_is_not(Token::Name k) 
  {
    return lookahead() != k;
  }

  bool
  Parse_context::nth_token_is(int n, Token::Name k) 
  {
    return lookahead(n) == k;
  }

  bool
  Parse_context::nth_token_is_not(int n, Token::Name k) 
  {
    return lookahead(n) != k;
  }

  /// If the lookahead matches n, then accept the token and update the 
  /// lookahead. Returns the matched token. Otherwise, this is a syntax 
  /// error.
  Token
  Parse_context::match(Token::Name n)
  {
    if (lookahead() == n)
      return consume();

    std::stringstream ss;
    ss << "syntax error";
    throw std::runtime_error(ss.str());
  }

  /// If lookahead matches n, then accept the token and update. Otherwise, 
  /// no action is performed, and eof token is returned.
  Token
  Parse_context::match_if(Token::Name n)
  {
    if (lookahead() == n)
      return consume();
    else
      return {};
  }

  Token
  Parse_context::require(Token::Name n)
  {
    assert(next_token_is(n));
    return consume();
  }

  /// Consumes the current token, returning it.
  Token
  Parse_context::consume()
  {
    Token tok = peek();
    m_tok.pop_front();
    if (m_tok.empty())
      fetch();
    return tok;
  }

  const Token&
  Parse_context::peek()
  {
    assert(!m_tok.empty());
    return m_tok.front();
  }

  void
  Parse_context::fetch()
  {
    m_tok.push_back(m_lex());
  }

  /// Consume tokens until we reach the next non-nested token with name `n`.
  /// That matching token is not consumed.
  ///
  /// A nested token is one that occurs within parentheses, braces, or 
  /// brackets. For example, given a sequence "{ expr; } expr;", 
  /// `consume_to(Token::semicolon` will match the last semicolon, not the
  /// first.
  void
  Parse_context::consume_to(Token_seq& toks, Token::Name n)
  {
    int nesting = 0;
    while (next_token_is_not(n)) {
      switch (lookahead()) {
      default:
        break;
      case Token::lparen:
      case Token::lbrace:
      case Token::lbracket:
        ++nesting;
        break;
      
      case Token::rparen:
      case Token::rbrace:
      case Token::rbracket:
        --nesting;
        break;
      }
      toks.push_back(consume());
    }
    assert(next_token_is(n));
  }

  /// Consume all of the tokens up to and including `n`.
  void
  Parse_context::consume_thru(Token_seq& toks, Token::Name n)
  {
    consume_to(toks, n);
    toks.push_back(consume());
  }

  Token_seq
  Parse_context::consume_to(Token::Name n)
  {
    Token_seq toks;
    consume_to(toks, n);
    return toks;
  }

  Token_seq
  Parse_context::consume_thru(Token::Name n)
  {
    Token_seq toks;
    consume_thru(toks, n);
    return toks;
  }

  /// Insert the tokens into the token stream so that they are the next
  /// tokens lexed.
  void
  Parse_context::inject(const Token_seq& toks)
  {
    m_tok.insert(m_tok.begin(), toks.begin(), toks.end());
  }

  // ------------------------------------------------------------------------ //
  // Declarative regions

  Parsing_declarative_region::
  Parsing_declarative_region(Parser& p, Declaration* d)
    : m_sema(p.get_semantics()), m_cons(d)
  {
    m_sema.enter_scope(d);
  }

  Parsing_declarative_region::
  Parsing_declarative_region(Parser& p, Statement* s)
    : m_sema(p.get_semantics()), m_cons(s)
  {
    m_sema.enter_scope(s);
  }

  Parsing_declarative_region::~Parsing_declarative_region()
  {
    if (m_cons.is_declaration())
      m_sema.leave_scope(m_cons.get_declaration());
    else if (m_cons.is_statement())
      m_sema.leave_scope(m_cons.get_statement());
    else
      __builtin_unreachable();
  }

  Restored_declarative_region::
  Restored_declarative_region(Parser& p, Declaration* d)
    : m_sema(p.get_semantics()), m_decl(d)
  {
    m_sema.restore_scope(m_decl);
  }

  Restored_declarative_region::
  ~Restored_declarative_region()
  {
    m_sema.empty_scope(m_decl);
  }

} // namespace beaker