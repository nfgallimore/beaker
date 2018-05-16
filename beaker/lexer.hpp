#pragma once

#include <beaker/token.hpp>

#include <unordered_map>

namespace beaker
{
  class File;
  class Context;

  /// The lexer is responsible for transforming input characters into output
  /// tokens. Note that whitespace is excluded from the output (i.e., it is
  /// not significant to syntactic and semantic analysis).
  ///
  /// \todo The lexer needs to accept input other than files. 
  ///
  /// \todo Create sublexer classes for identifiers, numbers, characters, and
  /// strings. These could be useful for different applications.
  class Lexer
  {
  public:
    Lexer(Context& cxt, const File& f);

    Token operator()() { return scan(); }

  private:
    Token scan();

    bool eof() const;

    char peek() const;
    char peek(int n) const;

    char advance();
    char accept();
    void accept(int n);
    char ignore();
    void ignore(int n);

    /// Returns the current location
    Location get_location() const;

    // Skip functions -- advance past certain lexical constructs
    void skip_space();
    void skip_newline();
    void skip_comment();

    // Accept functions -- yield a token.
    Token lex_monograph(Token::Name n);
    Token lex_digraph(Token::Name n);
    Token lex_trigraph(Token::Name n);
    Token lex_word();
    Token lex_number();
    Token lex_binary_number();
    Token lex_hexadecimal_number();
    Token lex_character();
    Token lex_string();

    char scan_escape_sequence();
    char scan_character_char();
    char scan_string_char();

  private:
    /// Used to create symbols.
    Context& m_cxt;

    /// The base offset for constructing locations. This is -1 if lexing
    /// from a source outside of a file.
    unsigned m_base;

    // Lex state
    
    /// The absolute beginning of a lexed buffer.
    const char* m_begin;

    /// The current position in the buffer.
    const char* m_curr;

    /// The position past the end of the buffer.
    const char* m_end;

    /// The location of the current token.
    Location m_loc;

    /// Stores information about reserved words.
    std::unordered_map<Symbol, Token::Name> m_reserved;
  };

} // namespace beaker
