#include "lexer.hpp"
#include "context.hpp"
#include "file.hpp"

#include <cassert>
#include <cctype>
#include <sstream>
#include <iostream>

namespace beaker
{
  // ------------------------------------------------------------------------ //
  // Character classes

  static bool
  is_space(char c)
  {
    return c == ' ' || c == '\t';
  }

  static bool
  is_newline(char c)
  {
    return c == '\n';
  }

  static bool
  is_nondigit(char c)
  {
    return std::isalpha(c) || c == '_';
  }

  static bool
  is_digit(char c)
  {
    return std::isdigit(c);
  }

  static bool
  is_alphanumeric(char c)
  {
    return std::isalnum(c);
  }

  static bool
  is_binary_digit(char c)
  {
    return c == '0' || c == '1';
  }

  static bool
  is_hexadecimal_digit(char c)
  {
    return std::isxdigit(c);
  }

  // ------------------------------------------------------------------------ //
  // Lexer

  static const char*
  get_start_of_input(const File& f)
  {
    return f.get_text().data();
  }

  static const char*
  get_end_of_input(const File& f)
  {
    return f.get_text().data() + f.get_text().size();
  }

  Lexer::Lexer(Context& cxt, const File& f)
    : m_cxt(cxt), 
      m_base(f.get_base_offset()),
      m_begin(get_start_of_input(f)), 
      m_curr(m_begin), 
      m_end(get_end_of_input(f)), 
      m_loc()
  {
    m_reserved.insert({
      {m_cxt.get_symbol("auto"), Token::auto_kw},
      {m_cxt.get_symbol("bool"), Token::bool_kw},
      {m_cxt.get_symbol("break"), Token::break_kw},
      {m_cxt.get_symbol("case"), Token::case_kw},
      {m_cxt.get_symbol("char"), Token::char_kw},
      {m_cxt.get_symbol("char8_t"), Token::char8_t_kw},
      {m_cxt.get_symbol("char16_t"), Token::char16_t_kw},
      {m_cxt.get_symbol("char32_t"), Token::char32_t_kw},
      {m_cxt.get_symbol("concept"), Token::concept_kw},
      {m_cxt.get_symbol("const"), Token::const_kw},
      {m_cxt.get_symbol("continue"), Token::continue_kw},
      {m_cxt.get_symbol("default"), Token::default_kw},
      {m_cxt.get_symbol("delete"), Token::delete_kw},
      {m_cxt.get_symbol("do"), Token::do_kw},
      {m_cxt.get_symbol("double"), Token::double_kw},
      {m_cxt.get_symbol("else"), Token::else_kw},
      {m_cxt.get_symbol("enum"), Token::enum_kw},
      {m_cxt.get_symbol("export"), Token::export_kw},
      {m_cxt.get_symbol("extern"), Token::extern_kw},
      {m_cxt.get_symbol("false"), Token::false_kw},
      {m_cxt.get_symbol("float"), Token::float_kw},
      {m_cxt.get_symbol("for"), Token::for_kw},
      {m_cxt.get_symbol("func"), Token::func_kw},
      {m_cxt.get_symbol("goto"), Token::goto_kw},
      {m_cxt.get_symbol("if"), Token::if_kw},
      {m_cxt.get_symbol("int"), Token::int_kw},
      {m_cxt.get_symbol("int8"), Token::int8_kw},
      {m_cxt.get_symbol("int16"), Token::int16_kw},
      {m_cxt.get_symbol("int32"), Token::int32_kw},
      {m_cxt.get_symbol("int64"), Token::int64_kw},
      {m_cxt.get_symbol("int128"), Token::int128_kw},
      {m_cxt.get_symbol("namespace"), Token::namespace_kw},
      {m_cxt.get_symbol("new"), Token::new_kw},
      {m_cxt.get_symbol("operator"), Token::operator_kw},
      {m_cxt.get_symbol("requires"), Token::requires_kw},
      {m_cxt.get_symbol("return"), Token::return_kw},
      {m_cxt.get_symbol("switch"), Token::switch_kw},
      {m_cxt.get_symbol("template"), Token::template_kw},
      {m_cxt.get_symbol("true"), Token::true_kw},
      {m_cxt.get_symbol("typename"), Token::typename_kw},
      {m_cxt.get_symbol("union"), Token::union_kw},
      {m_cxt.get_symbol("unit"), Token::union_kw},
      {m_cxt.get_symbol("using"), Token::using_kw},
      {m_cxt.get_symbol("val"), Token::val_kw},
      {m_cxt.get_symbol("var"), Token::var_kw},
      {m_cxt.get_symbol("virtual"), Token::virtual_kw},
      {m_cxt.get_symbol("volatile"), Token::volatile_kw},
      {m_cxt.get_symbol("while"), Token::while_kw},
    });
  }

  // Returns true if past the end of file.
  bool
  Lexer::eof() const
  {
    return m_curr == m_end;
  }

  // Returns the current character or 0, if past the end of file.
  char
  Lexer::peek() const 
  { 
    return eof() ? 0 : *m_curr; 
  }

  // Returns the nth character past the current character or 0 if past the
  // end of file.
  char 
  Lexer::peek(int n) const 
  { 
    if (n < m_end - m_curr)
      return *(m_curr + n);
    else
      return 0;
  }

  char
  Lexer::advance()
  {
    return *m_curr++;
  }

  char
  Lexer::accept()
  {
    return advance();
  }

  void
  Lexer::accept(int n)
  {
    while (n) {
      accept();
      --n;
    }
  }

  char
  Lexer::ignore()
  {
    return advance();
  }

  void
  Lexer::ignore(int n)
  {
    while (n) {
      accept();
      --n;
    }
  }

  Location
  Lexer::get_location() const
  {
    return Location(m_base + (m_curr - m_begin));
  }

  Token
  Lexer::scan()
  {
    while (!eof()) {
      // Note the start location of the token.
      m_loc = get_location();

      // Analyze the current character.
      switch (*m_curr) {
      case ' ':
      case '\t':
        skip_space();
        continue;

      case '\n':
        skip_newline();
        continue;

      case '#':
        skip_comment();
        continue;

      case '(':
        return lex_monograph(Token::lparen);
      
      case ')':
        return lex_monograph(Token::rparen);
      
      case '[':
        return lex_monograph(Token::lbracket);
      
      case ']':
        return lex_monograph(Token::rbracket);
      
      case '{':
        return lex_monograph(Token::lbrace);
      
      case '}':
        return lex_monograph(Token::rbrace);
      
      case ',':
        return lex_monograph(Token::comma);
      
      case ';':
        return lex_monograph(Token::semicolon);
      
      case ':':
        return lex_monograph(Token::colon);

      case '<':
        if (peek(1) == '=') 
          return lex_digraph(Token::less_equal);
        if (peek(1) == '<')
          return lex_digraph(Token::less_less);
        return lex_monograph(Token::less);

      case '>':
        if (peek(1) == '=') 
          return lex_digraph(Token::greater_equal);
        if (peek(1) == '>')
          return lex_digraph(Token::greater_greater);
        return lex_monograph(Token::greater);

      case '=':
        if (peek(1) == '=') 
          return lex_digraph(Token::equal_equal);
        else
          return lex_monograph(Token::equal);

      case '+':
        return lex_monograph(Token::plus);
      
      case '-':
        if (peek(1) == '>')
          return lex_digraph(Token::arrow);
        return lex_monograph(Token::minus);
      
      case '*':
        return lex_monograph(Token::star);
      
      case '/':
        return lex_monograph(Token::slash);
      
      case '%':
        return lex_monograph(Token::percent);

      case '&':
        return lex_monograph(Token::ampersand);

      case '|':
        return lex_monograph(Token::bar);

      case '^':
        return lex_monograph(Token::caret);

      case '~':
        return lex_monograph(Token::tilde);

      case '?':
        return lex_monograph(Token::question);

      case '\'':
        return lex_character();
        
      case '"':
        return lex_string(); 

      default: {
        if (is_nondigit(*m_curr))
          return lex_word();
        
        if (is_digit(*m_curr))
          return lex_number();

        std::stringstream ss;
        ss << "invalid character '" << *m_curr << '\'';
        throw std::runtime_error(ss.str());
      }
      }
    }
    return {};
  }


  void
  Lexer::skip_space()
  {
    assert(is_space(*m_curr));
    ignore();
    while (!eof() && is_space(*m_curr))
      ignore();
  }

  void
  Lexer::skip_newline()
  {
    assert(*m_curr == '\n');
    ignore();
  }

  void
  Lexer::skip_comment()
  {
    assert(*m_curr == '#');
    ignore();
    while (!eof() && !is_newline(*m_curr))
      ignore();
  }

  Token
  Lexer::lex_monograph(Token::Name n)
  {
    accept();
    return Token(n, m_loc, 1);
  }

  Token
  Lexer::lex_digraph(Token::Name n)
  {
    accept(2);
    return Token(n, m_loc, 2);
  }

  Token
  Lexer::lex_trigraph(Token::Name n)
  {
    accept(3);
    return Token(n, m_loc, 3);
  }

  Token
  Lexer::lex_word()
  {
    assert(is_nondigit(*m_curr));
    
    const char* start = m_curr;
    accept();
    while (!eof() && is_alphanumeric(*m_curr))
      accept();

    // Determine whether [start, m_curr) is an identifier or a reserved word.
    std::string str(start, m_curr);
    Symbol sym = m_cxt.get_symbol(str);
    auto iter = m_reserved.find(sym);
    if (iter != m_reserved.end())
      return Token(iter->second, m_loc, sym->size());
    else 
      return Token(Token::identifier, m_loc, sym);
  }

  Token
  Lexer::lex_number()
  {
    assert(isdigit(*m_curr));
    const char* start = m_curr;

    // Check for a prefix; that determines the alphabet of the subsequent lex.
    if (*m_curr == '0') {
      char pre = peek(1);
      switch (pre) {
        case 'b': case 'B':
          return lex_binary_number();
        case 'x': case 'X':
          return lex_hexadecimal_number();
        default:
          break;
      }
    }

    // This is a decimal whole number. We detect fractions later.
    accept();
    while (!eof() && is_digit(*m_curr))
      accept();

    // The integer is in [start, m_curr).
    if (peek() != '.') {
      std::string str(start, m_curr);
      Symbol sym = m_cxt.get_symbol(str);
      return Token(Token::decimal_integer, m_loc, sym);
    }

    // Lex the fractional component.
    //
    // FIXME: There must be at least one digit.
    accept();
    while (!eof() && is_digit(*m_curr))
      accept();

    // FIXME: Lex the exponent.

    // The floating point number is in [start, m_curr).
    std::string str(start, m_curr);
    Symbol sym = m_cxt.get_symbol(str);
    return Token(Token::decimal_float, m_loc, sym);
  }

  Token
  Lexer::lex_binary_number()
  {
    // Skip the prefix; we don't need to to compute the value.
    accept(2);

    // FIXME: There must be at least one digit.

    const char* start = m_curr;
    while (!eof() && is_binary_digit(*m_curr))
      accept();

    std::string str(start, m_curr);
    Symbol sym = m_cxt.get_symbol(str);
    return Token(Token::binary_integer, m_loc, sym);
  }

  Token
  Lexer::lex_hexadecimal_number()
  {
    // Skip the prefix; we don't need to to compute the value.
    accept(2);

    // FIXME: There must be at least one digit.

    const char* start = m_curr;
    while (!eof() && is_hexadecimal_digit(*m_curr))
      accept();

    std::string str(start, m_curr);
    Symbol sym = m_cxt.get_symbol(str);
    return Token(Token::hexadecimal_integer, m_loc, sym);
  }

  static bool
  is_character_character(char c)
  {
    return c != '\n' && c != '\'';
  }

  char
  Lexer::scan_escape_sequence()
  {
    assert(*m_curr == '\\');
    accept();
    if (eof())
      throw std::runtime_error("unterminated escape-sequence");
    switch (*m_curr) { // Note the increment.
    case '\'':
    case '\"':
    case '\\':
    case 'a':
    case 'b':
    case 'f':
    case 'n':
    case 'r':
    case 't':
    case 'v':
      return accept();
    default:
      throw std::runtime_error("invalid escape-sequence");
    }
  }

  char
  Lexer::scan_character_char()
  {
    /// FIXME: Actually validate the character.
    return accept();
  }

  Token
  Lexer::lex_character()
  {
    assert(*m_curr == '\'');
    const char* start = m_curr;
    accept();
    while (!eof() && *m_curr != '\'') {
      if (*m_curr == '\\')
        scan_escape_sequence();
      else
        scan_character_char();
    }
    if (eof())
      throw std::runtime_error("unterminated character-literal");
    accept();

    // The character literal is in [start, m_curr).
    std::string str(start, m_curr);
    Symbol sym = m_cxt.get_symbol(str);
    return Token(Token::character, m_loc, sym);
  }

  char
  Lexer::scan_string_char()
  {
    // FIXME: Actually validate the characters.
    return accept();
  }

  Token
  Lexer::lex_string()
  {
    assert(*m_curr == '"');
    accept();

    const char* start = m_curr;
    accept();
    while (!eof() && *m_curr != '"') {
      if (*m_curr == '\\')
        scan_escape_sequence();
      else
        accept();
    }
    if (eof())
      throw std::runtime_error("unterminated string-literal");
    accept();

    // The string literal is in [start, m_curr).
    std::string str(start, m_curr);
    Symbol sym = m_cxt.get_symbol(str);
    return Token(Token::string, m_loc, sym);
  }

} // namespace beaker