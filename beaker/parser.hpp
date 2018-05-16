#pragma once

#include <beaker/common.hpp>
#include <beaker/lexer.hpp>
#include <beaker/semantics.hpp>

#include <deque>
#include <vector>

namespace beaker
{
  /// The shared context of all parsers.
  class Parse_context
  {
  public:
    Parse_context(Context& cxt, const File& f);

    /// Returns the underlying lexer.
    Lexer& get_lexer() { return m_lex; }

    /// Returns the semantic actions.
    Semantics &get_semantics() { return m_act; }

    /// Returns the lookahead token.
    const Token& peek();

    /// Returns the name of the lookahead token.
    Token::Name lookahead();
    
    /// Returns the name of the nth lookahead token.
    Token::Name lookahead(int n);
    
    /// Returns true if the next token is `n`.
    bool next_token_is(Token::Name k);

    /// Returns true if the next token is not `n`.
    bool next_token_is_not(Token::Name k);

    /// Returns true if the nth token is `n`.
    bool nth_token_is(int n, Token::Name k);

    /// Returns true if the nth token is not `n`.
    bool nth_token_is_not(int n, Token::Name k);

    Token match(Token::Name n);
    Token match_if(Token::Name n);
    Token require(Token::Name n);

    Token consume();

    Token_seq consume_to(Token::Name n);
    Token_seq consume_thru(Token::Name n);

    void consume_to(Token_seq& toks, Token::Name n);
    void consume_thru(Token_seq& toks, Token::Name n);

    void inject(const Token_seq& toks);

  private:
    void fetch();

  private:
    /// The lexer. This returns one token at a time.
    Lexer m_lex;
    
    // The semantic actions for parsing.
    Semantics m_act;

    /// Stores the sequence of lookahead tokens. This typically holds a single
    /// token unless we've looked beyond that.
    std::deque<Token> m_tok; 
  };


  /// The base class of all deferred parsing actions. 
  class Deferred_parse
  {
  protected:
    Deferred_parse(Declaration* decl, Token_seq&& toks)
      : m_decl(decl), m_toks(std::move(toks))
    { }

  public:
    virtual ~Deferred_parse() = default;

    /// Called to invoke the parsing action. 
    virtual void parse() = 0;

  protected:
    Declaration* m_decl;
    Token_seq m_toks;
  };


  /// A base class that provides facilities for more specific parsers. This
  /// class simply delegates a number of common operations to its derived
  /// classes. Note that all members are protected.
  class Parser
  {
  protected:
    Parser(Parse_context& cxt)
      : m_cxt(cxt), m_act(cxt.get_semantics())
    { }

  public:
    /// Returns the semantic actions for the parser.
    Semantics& get_semantics() const { return m_act; }

    /// Returns the current token.
    const Token& peek() { return m_cxt.peek(); }

    /// Returns the name of the next token.
    Token::Name lookahead() { return m_cxt.lookahead(); }

    /// Returns the name of the nth lookahead token.
    Token::Name lookahead(int n) { return m_cxt.lookahead(n); }
    
    /// Returns true if the name of the next token is `k`.
    bool next_token_is(Token::Name k) { return m_cxt.next_token_is(k); }
    
    /// Returns true if the name of the next token is not `k`.
    bool next_token_is_not(Token::Name k) { return m_cxt.next_token_is_not(k); }

    /// Returns true if the nth token is `k`.
    bool nth_token_is(int n, Token::Name k) { return m_cxt.nth_token_is(n, k); }

    /// Returns true if the nth token is not `k`.
    bool nth_token_is_not(int n, Token::Name k) { return m_cxt.nth_token_is_not(n, k); }

    /// Of the next token is `n`, accept it, and return the token. Otherwise,
    /// returns end-of-file and diagnoses a syntax error.
    Token match(Token::Name n) { return m_cxt.match(n); }

    /// Of the next token is `n`, accept it, and return the token. Otherwise,
    /// returns end-of-file, and returns nothing.
    Token match_if(Token::Name n) { return m_cxt.match_if(n); }

    /// Of the next token is `n`, accept it, and return the token. Otherwise,
    /// behavior is undefined.
    Token require(Token::Name n) { return m_cxt.require(n); }

    /// Consume the current token.
    Token consume() { return m_cxt.consume(); }

    /// Consume tokens up to but not including `n`.
    Token_seq consume_to(Token::Name n) { return m_cxt.consume_to(n); }

    /// Consume tokens up to and including `n`.
    Token_seq consume_thru(Token::Name n) { return m_cxt.consume_thru(n); }

    /// Consume tokens up to but not including `n`.
    void consume_to(Token_seq& toks, Token::Name n) { return m_cxt.consume_to(toks, n); }
    
    /// Consume tokens up to and including `n`.
    void consume_thru(Token_seq& toks, Token::Name n)  { return m_cxt.consume_thru(toks, n); }

    /// Injects tokens into the token stream.
    void inject(const Token_seq& toks) { return m_cxt.inject(toks); }

  protected:
    /// The shared parse state.
    Parse_context& m_cxt;

    /// The semantic actions. This reference is rebound to allow derived
    /// parsers direct access to the object.
    Semantics& m_act;
  };


  /// A helper class that constructs a declarative region over a particular
  /// set of matching functions. This ensures that the associated syntax is
  /// available for semantic actions when parsing the contents of that
  /// region.
  class Parsing_declarative_region
  {
  public:
    Parsing_declarative_region(Parser& p, Declaration* d);
    Parsing_declarative_region(Parser& p, Statement* s);
    ~Parsing_declarative_region();

  private:
    /// The semantic actions for the parser.
    Semantics& m_sema;

    /// The construct associated with the declarative region.
    Construct m_cons;
  };


} // namespace beaker
