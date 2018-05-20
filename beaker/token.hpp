#pragma once

#include <beaker/common.hpp>
#include <beaker/symbol.hpp>
#include <beaker/location.hpp>

#include <cassert>
#include <iosfwd>
#include <utility>

namespace beaker
{
  /// A token is a symbol in the language. Tokens represent occurrences of
  /// keywords, punctuators, operators, identifiers, and literals in source 
  /// code. A meta-token is a sequence of tokens that is interpreted as a
  /// single bit of syntax.
  ///
  /// The token class is intended to be passed by value.
  class Token
  {
  public:
    enum Name
    {
      // End-of-file
      eof,

      // Identifiers [lex.id]
      identifier,

      // Operators and punctuators [lex.operators]
      lbrace,
      rbrace,
      lbracket,
      rbracket,
      lparen,
      rparen,
      semicolon,
      colon,
      ellipsis,
      question,
      colon_colon,
      dot,
      plus,
      minus,
      star,
      slash,
      percent,
      caret,
      ampersand,
      bar,
      tilde,
      bang,
      equal,
      less,
      greater,
      plus_equal,
      minus_equal,
      star_equal,
      slash_equal,
      percent_equal,
      caret_equal,
      ampersand_equal,
      bar_equal,
      less_less,
      greater_greater,
      less_less_equal,
      greater_greater_equal,
      equal_equal,
      bang_equal,
      less_equal,
      greater_equal,
      ampersand_ampersand,
      bar_bar,
      comma,
      arrow,

      // Keywords [lex.key]
      auto_kw,
      bool_kw,
      break_kw,
      case_kw,
      char_kw,
      char8_t_kw,
      char16_t_kw,
      char32_t_kw,
      concept_kw,
      const_kw,
      continue_kw,
      default_kw,
      delete_kw,
      do_kw,
      double_kw,
      else_kw,
      enum_kw,
      export_kw,
      extern_kw,
      false_kw,
      float_kw,
      for_kw,
      func_kw,
      goto_kw,
      if_kw,
      int_kw,
      int8_kw,
      int16_kw,
      int32_kw,
      int64_kw,
      int128_kw,
      namespace_kw,
      new_kw,
      operator_kw,
      ref_kw,
      requires_kw,
      return_kw,
      switch_kw,
      template_kw,
      true_kw,
      typename_kw,
      union_kw,
      unit_kw,
      using_kw,
      virtual_kw,
      val_kw,
      var_kw,
      volatile_kw,
      while_kw,

      // Literals
      binary_integer,
      octal_integer,
      decimal_integer,
      hexadecimal_integer,
      decimal_float,
      hexadecimal_float,
      character,
      string,
      raw_string,
    };

    /// Constructs the end-of-file token.
    Token() 
      : m_name(eof), m_loc(), m_attr() 
    { }

    /// Construct an abstract basic token with name `n`.
    Token(Name n) 
      : Token(n, Location(), get_token_length(n)) 
    { }

    /// Construct a basic token with name `n` and location `loc`.
    Token(Name n, Location loc) 
      : Token(n, loc, get_token_length(n)) 
    { }

    /// Construct a basic token with name, location, and length.
    Token(Name n, Location loc, int len);

    /// Construct an abstract identifier or literal with spelling `sym`.
    Token(Name n, Symbol sym)
      : Token(n, Location(), sym)
    { }

    /// Construct an identifier or literal with name, location, and spelling.
    Token(Name n, Location loc, Symbol sym);

    /// Construct a meta token.
    Token(Name n, std::initializer_list<Token> list);

    /// Release any resources acquired by the token.
    ~Token();

    // Operators

    /// Returns true if this is not the end-of-file token.
    explicit operator bool() const { return !is_eof(); }

    // Token name

    /// Returns the name of the token.
    Name get_name() const { return m_name; }

    /// Returns true if this denotes the end-of-file.
    bool is_eof() const { return m_name == eof; }

    /// Returns true if this is an identifier.
    bool is_identifier() const { return m_name == identifier; }

    /// Returns true if this is a punctuator, operator, or keyword.
    bool is_basic() const;

    /// Returns true if this is a punctuator or operator.
    bool is_punctuator_or_operator() const;

    /// Returns true if this is an overloadable operator.
    bool is_operator() const;

    /// Returns true if this is a keyword or alternative token.
    bool is_keyword() const;

    /// Returns true if this is a literal.
    bool is_literal() const;

    /// Returns true if this is a numeric literal.
    bool is_number() const;

    /// Returns true if this is a text literal.
    bool is_text() const;

    /// Returns true if this is a meta token.
    bool is_meta() const;

    /// Returns true if this token has the name `n`.
    bool is(Name n) const { return m_name == n; }

    /// Returns true if this token has some name other than `n`.
    bool is_not(Name n) const { return !is(n); }

    // Attributes

    /// Returns the location of the token.
    Location get_location() const { return m_loc; }

    /// Returns the length of the token.
    int get_length() const;

    /// Returns a null-terminated C-string representing the token.
    const char* get_spelling() const;

    /// Returns the symbol for identifiers and literals.
    Symbol get_symbol() const;

    /// Returns the length of basic tokens.
    static int get_token_length(Name n);

    /// Returns the spelling of basic tokens.
    static const char* get_token_spelling(Name n);

  private:
    /// The name (or kind) of the token. This indicates the symbol present
    /// in the source file.
    Name m_name;

    /// The location of the token in the source text.
    Location m_loc;

    /// An opaque reference to data associated with the token. The kind of
    /// value depends on the token as follows:
    ///
    ///   identifiers -- the uniqued symbol
    ///   literals -- the unique'd symbol
    ///   otherwise -- the length of the symbol.
    std::uintptr_t m_attr;
  };

  inline
  Token::Token(Name n, Location loc, int len)
    : m_name(n), m_loc(loc), m_attr(len)
  {
    assert(len == get_token_length(n));
    assert(is_basic());
  }

  inline
  Token::Token(Name n, Location loc, Symbol sym)
    : m_name(n), m_loc(loc), m_attr((std::uintptr_t)sym)
  {
    assert(is_identifier() || is_literal());
  }

  inline bool
  Token::is_basic() const
  {
    return is_punctuator_or_operator() || is_keyword() || is_eof();
  }

  inline bool
  Token::is_punctuator_or_operator() const
  { 
    return m_name >= lbrace && m_name <= arrow;
  }

  inline bool
  Token::is_keyword() const
  {
    return m_name >= auto_kw && m_name <= while_kw;
  }

  inline bool
  Token::is_literal() const
  {
    return m_name >= binary_integer && m_name <= raw_string;
  }

  inline bool
  Token::is_number() const
  {
    return m_name >= binary_integer && m_name <= hexadecimal_float;
  }

  inline bool
  Token::is_text() const
  {
    return m_name >= character && m_name <= raw_string;
  }

  inline bool
  Token::is_meta() const
  {
    return false;
  }

  std::ostream& operator<<(std::ostream& os, const Token& tok);

  // ------------------------------------------------------------------------ //
  // Token sequence

  using Token_seq = std::vector<Token>;

  // ------------------------------------------------------------------------ //
  // Token factory

  /// Creates tokens in the absence of a source file. 
  ///
  /// \todo Add more factory methods for basic tokens.
  class Token_factory
  {
  public:
    Token_factory(Symbol_table& syms)
      : m_syms(syms)
    { }

    /// Returns the end-of-file token.
    Token make_eof() { return Token(); }

    /// Returns a punctuator or operator
    Token make_token(Token::Name n) { return Token(n); }
    Token make_lparen() { return Token(Token::lparen); }
    Token make_rparen() { return Token(Token::rparen); }
    Token make_lbrace() { return Token(Token::lbrace); }
    Token make_rbrace() { return Token(Token::rbrace); }
    Token make_lbracket() { return Token(Token::lbracket); }
    Token make_rbracket() { return Token(Token::lbracket); }
    Token make_semicolon() { return Token(Token::semicolon); }
    Token make_colon_colon() { return Token(Token::colon_colon); }
    Token make_comma() { return Token(Token::comma); }

    // Keywords
    Token make_auto() { return Token(Token::auto_kw); }
    Token make_bool() { return Token(Token::bool_kw); }
    Token make_char() { return Token(Token::char_kw); }
    Token make_char8_t() { return Token(Token::char8_t_kw); }
    Token make_char16_t() { return Token(Token::char16_t_kw); }
    Token make_char32_t() { return Token(Token::char32_t_kw); }
    Token make_const() { return Token(Token::const_kw); }
    Token make_double() { return Token(Token::double_kw); }
    Token make_extern() { return Token(Token::extern_kw); }
    Token make_float() { return Token(Token::float_kw); }
    Token make_int() { return Token(Token::int_kw); }
    Token make_return() { return Token(Token::return_kw); }
    Token make_virtual() { return Token(Token::virtual_kw); }
    Token make_volatile() { return Token(Token::volatile_kw); }
    
    Token make_identifier(const char* str);
    
    Token make_integer(int n, Radix rad = Radix::decimal);
    Token make_integer(const char* str, Radix rad = Radix::decimal);
    Token make_integer(Symbol sym, Radix rad = Radix::decimal);
    
    Token make_float(double n, Radix rad = Radix::decimal);
    Token make_character(char c, Encoding enc = Encoding::basic);
    Token make_string(const char* str, Encoding enc = Encoding::basic);
    Token make_raw_string(const char* str, Encoding enc = Encoding::basic);

  private:
    Symbol_table &m_syms;
  };

} // namespace beaker
