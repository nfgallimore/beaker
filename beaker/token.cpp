#include "token.hpp"

#include <cstring>
#include <sstream>
#include <bitset>

namespace beaker
{
  Token::Token(Name n, std::initializer_list<Token> toks)
    : m_name(n), m_attr()
  {
    assert(is_meta());
    Token* arr = new Token[toks.size()];
    std::copy(toks.begin(), toks.end(), arr);
    m_attr = (std::uintptr_t)arr;
  }

  Token::~Token()
  {
    if (is_meta()) {
      Token* arr = (Token*)m_attr;
      delete[] arr;
    }
  }

  bool 
  Token::is_operator() const
  {
    switch (m_name) {
    default:
      return false;
    case plus:
    case minus:
    case star:
    case slash:
    case percent:
    case caret:
    case ampersand:
    case bar:
    case tilde:
    case bang:
    case equal:
    case less:
    case greater:
    case plus_equal:
    case minus_equal:
    case star_equal:
    case slash_equal:
    case percent_equal:
    case caret_equal:
    case ampersand_equal:
    case bar_equal:
    case less_less:
    case greater_greater:
    case less_less_equal:
    case greater_greater_equal:
    case equal_equal:
    case bang_equal:
    case less_equal:
    case greater_equal:
    case ampersand_ampersand:
    case bar_bar:
    case comma:
    case arrow:

    case new_kw:
    case delete_kw:
      return true;
    }
  }

  int
  Token::get_length() const
  {
    if (is_basic())
      return (int)m_attr;
    else
      return ((Symbol)m_attr)->size();
  }

  const char*
  Token::get_spelling() const
  {
    if (is_basic())
      return get_token_spelling(m_name);
    else
      return ((Symbol)m_attr)->c_str();
  }

  Symbol
  Token::get_symbol() const
  {
    assert(!is_basic());
      return (Symbol)m_attr;
  }

  int
  Token::get_token_length(Name n)
  {
    switch (n) {
    case eof: return 0;
    case lbrace: return 1;
    case rbrace: return 1;
    case lbracket: return 1;
    case rbracket: return 1;
    case lparen: return 1;
    case rparen: return 1;
    case semicolon: return 1;
    case colon: return 1;
    case ellipsis: return 3;
    case question: return 1;
    case colon_colon: return 2;
    case dot: return 1;
    case plus: return 1;
    case minus: return 1;
    case star: return 1;
    case slash: return 1;
    case percent: return 1;
    case caret: return 1;
    case ampersand: return 1;
    case bar: return 1;
    case tilde: return 1;
    case bang: return 1;
    case equal: return 1;
    case less: return 1;
    case greater: return 1;
    case plus_equal: return 2;
    case minus_equal: return 2;
    case star_equal: return 2;
    case slash_equal: return 2;
    case percent_equal: return 2;
    case caret_equal: return 2;
    case ampersand_equal: return 2;
    case bar_equal: return 2;
    case less_less: return 2;
    case greater_greater: return 2;
    case less_less_equal: return 3;
    case greater_greater_equal: return 3;
    case equal_equal: return 2;
    case bang_equal: return 2;
    case less_equal: return 2;
    case greater_equal: return 2;
    case ampersand_ampersand: return 2;
    case bar_bar: return 2;
    case comma: return 1;
    case arrow: return 2;

    // Keywords [lex.key]
    case auto_kw: return std::strlen("auto");
    case bool_kw: return std::strlen("bool");
    case break_kw: return std::strlen("break");
    case case_kw: return std::strlen("case");
    case char_kw: return std::strlen("char");
    case char8_t_kw: return std::strlen("char8_t");
    case char16_t_kw: return std::strlen("char16_t");
    case char32_t_kw: return std::strlen("char32_t");
    case concept_kw: return std::strlen("concept");
    case const_kw: return std::strlen("const");
    case continue_kw: return std::strlen("continue");
    case default_kw: return std::strlen("default");
    case delete_kw: return std::strlen("delete");
    case do_kw: return std::strlen("do");
    case double_kw: return std::strlen("double");
    case else_kw: return std::strlen("else");
    case enum_kw: return std::strlen("enum");
    case export_kw: return std::strlen("export");
    case extern_kw: return std::strlen("extern");
    case false_kw: return std::strlen("false");
    case float_kw: return std::strlen("float");
    case for_kw: return std::strlen("for");
    case func_kw: return std::strlen("func");
    case goto_kw: return std::strlen("goto");
    case if_kw: return std::strlen("if");
    case int_kw: return std::strlen("int");
    case namespace_kw: return std::strlen("namespace");
    case new_kw: return std::strlen("new");
    case operator_kw: return std::strlen("operator");
    case requires_kw: return std::strlen("requires");
    case return_kw: return std::strlen("return");
    case switch_kw: return std::strlen("switch");
    case template_kw: return std::strlen("template");
    case true_kw: return std::strlen("true");
    case typename_kw: return std::strlen("typename");
    case union_kw: return std::strlen("union");
    case unit_kw: return std::strlen("unit");
    case using_kw: return std::strlen("using");
    case val_kw: return std::strlen("val");
    case var_kw: return std::strlen("var");
    case virtual_kw: return std::strlen("virtual");
    case volatile_kw: return std::strlen("volatile");
    case while_kw: return std::strlen("while");

    default:
      __builtin_unreachable();
    }
  }

  const char*
  Token::get_token_spelling(Name n)
  {
    switch (n) {
    case eof: return "";
    case lbrace: return "{";
    case rbrace: return "}";
    case lbracket: return "[";
    case rbracket: return "]";
    case lparen: return "(";
    case rparen: return ")";
    case semicolon: return ";";
    case colon: return ":";
    case ellipsis: return "...";
    case question: return "?";
    case colon_colon: return "::";
    case dot: return ".";
    case plus: return "+";
    case minus: return "-";
    case star: return "*";
    case slash: return "/";
    case percent: return "%";
    case caret: return "^";
    case ampersand: return "&";
    case bar: return "|";
    case tilde: return "~";
    case bang: return "!";
    case equal: return "=";
    case less: return "<";
    case greater: return ">";
    case plus_equal: return "+=";
    case minus_equal: return "-=";
    case star_equal: return "*=";
    case slash_equal: return "/=";
    case percent_equal: return "%=";
    case caret_equal: return "^=";
    case ampersand_equal: return "&=";
    case bar_equal: return "|=";
    case less_less: return "<<";
    case greater_greater: return ">>";
    case less_less_equal: return "<<=";
    case greater_greater_equal: return ">>=";
    case equal_equal: return "==";
    case bang_equal: return "!=";
    case less_equal: return "<=";
    case greater_equal: return ">=";
    case ampersand_ampersand: return "&&";
    case bar_bar: return "||";
    case comma: return ",";
    case arrow: return "->";

    // Keywords [lex.key]
    case auto_kw: return "auto";
    case bool_kw: return "bool";
    case break_kw: return "break";
    case case_kw: return "case";
    case char_kw: return "char";
    case char8_t_kw: return "char8_t";
    case char16_t_kw: return "char16_t";
    case char32_t_kw: return "char32_t";
    case concept_kw: return "concept";
    case const_kw: return "const";
    case continue_kw: return "continue";
    case default_kw: return "default";
    case delete_kw: return "delete";
    case do_kw: return "do";
    case else_kw: return "else";
    case enum_kw: return "enum";
    case export_kw: return "export";
    case extern_kw: return "extern";
    case false_kw: return "false";
    case float_kw: return "float";
    case for_kw: return "for";
    case func_kw: return "func";
    case goto_kw: return "goto";
    case if_kw: return "if";
    case int_kw: return "int";
    case namespace_kw: return "namespace";
    case new_kw: return "new";
    case operator_kw: return "operator";
    case requires_kw: return "requires";
    case return_kw: return "return";
    case switch_kw: return "switch";
    case template_kw: return "template";
    case true_kw: return "true";
    case typename_kw: return "typename";
    case union_kw: return "union";
    case unit_kw: return "unit";
    case using_kw: return "using";
    case val_kw: return "val";
    case var_kw: return "var";
    case virtual_kw: return "virtual";
    case volatile_kw: return "volatile";
    case while_kw: return "while";

    default:
      __builtin_unreachable();
    }
  }

  std::ostream& 
  operator<<(std::ostream& os, const Token& tok)
  {
    if (!tok)
      return os << "end-of-file";
    else
      return os << tok.get_spelling();
  }


  // ------------------------------------------------------------------------ //
  // Token factory

  Token
  Token_factory::make_identifier(const char* str)
  {
    return {Token::identifier, m_syms.get(str)};
  }

  Token
  Token_factory::make_integer(int n, Radix rad)
  {
    std::stringstream ss;
    switch (rad) {
    case Radix::binary: {
      std::bitset<sizeof(int)> bits(n);
      ss << "0b" << bits;
      return {Token::binary_integer, m_syms.get(ss.str())};
    }
    
    case Radix::decimal:
      ss << n;
      return {Token::decimal_integer, m_syms.get(ss.str())};
    
    case Radix::hexadecimal:
      ss << "0x" << std::hex << n;
      return {Token::hexadecimal_integer, m_syms.get(ss.str())};
    
    default:
      __builtin_unreachable();
    }
  }

  Token
  Token_factory::make_integer(const char* str, Radix rad)
  {
    return make_integer(m_syms.get(str), rad);
  }

  static Token::Name
  get_integer_name(Radix rad)
  {
    switch (rad) {
    case Radix::binary:
      return Token::binary_integer;
    case Radix::decimal:
      return Token::decimal_integer;
    case Radix::hexadecimal:
      return Token::hexadecimal_integer;
    }
  }

  Token
  Token_factory::make_integer(Symbol sym, Radix rad)
  {
    // FIXME: Assert that `str` can be lexed as an integer with the
    // appropriate radix.
    return Token(get_integer_name(rad), sym);
  }

  Token
  Token_factory::make_float(double n, Radix rad)
  {
    assert(rad >= Radix::decimal);
    std::stringstream ss;
    if (rad == Radix::decimal) {
      ss << n;
      return {Token::decimal_float, m_syms.get(ss.str())};
    }
    else {
      ss << "0x" << std::hex << n;
      return {Token::hexadecimal_float, m_syms.get(ss.str())};
    }
  }

  // Write the encoding into the stringstream.
  static void
  emit_encoding(std::stringstream& ss, Encoding enc)
  {
    switch (enc) {
    case Encoding::basic:
      break;
    case Encoding::utf8:
      ss << "u8";
      break;
    case Encoding::utf16:
      ss << 'u';
      break;
    case Encoding::utf32:
      ss << 'U';
      break;
    }
  }

  // Write the character into the stringstream, escaping it if necessary. The
  // `str` parameter is true if escaping text for a string vs. a char.
  static void 
  emit_char(std::stringstream& ss, char ch, bool str)
  {
    switch (ch) {
    default:
      ss << ch;
      break;
    case '\'':
      ss << (str ? "\'" : "\\\'");
      break;
    case '\"':
      ss << (str ? "\\\"" : "\"");
      break;
    case '\\':
      ss << "\\\\";
      break;
    case '\a':
      ss << "\\a";
      break;
    case '\b':
      ss << "\\b";
      break;
    case '\f':
      ss << "\\f";
      break;
    case '\n':
      ss << "\\n";
      break;
    case '\r':
      ss << "\\r";
      break;
    case '\t':
      ss << "\\t";
      break;
    case '\v':
      ss << "\\v";
      break;
    }
  }

  Token
  Token_factory::make_character(char ch, Encoding enc)
  {
    std::stringstream ss;
    emit_encoding(ss, enc);
    ss << '\'';
    emit_char(ss, ch, false);
    ss << '\'';
    return {Token::character, m_syms.get(ss.str())};
  }

  Token
  Token_factory::make_string(const char* str, Encoding enc)
  {
    std::stringstream ss;
    emit_encoding(ss, enc);
    ss << '\"';
    while (*str != 0) {
      emit_char(ss, *str, false);
      ++str;
    }
    ss << '\"';
    return {Token::character, m_syms.get(ss.str())};
  }

  /// FIXME: This also needs to take delimiters as operands.
  Token
  Token_factory::make_raw_string(const char* str, Encoding enc)
  {
    __builtin_unreachable();
  }


} // namespace beaker
