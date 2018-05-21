#include "module_parser.hpp"
#include "type_parser.hpp"
#include "expression_parser.hpp"
#include "function_parser.hpp"
#include "data_parser.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace beaker
{
  /// module:
  ///   declaration-seq[opt]
  ///
  /// \todo We have the potential for  multiple potential top-level entities:
  /// modules, programs/drivers, REPL, etc. Should we have distinct semantic
  /// actions and parsers for each?
  Declaration*
  Module_parser::parse_module()
  {
    Declaration* tu = m_act.on_start_translation();

    // Parse top-level structures.
    {
      Parsing_declarative_region region(*this, tu);
      parse_declaration_seq();
    }

    // Parse deferred structures.
    parse_deferred_declarations();
    parse_deferred_definitions();

    return m_act.on_finish_translation(tu);
  }

  /// declaration-seq
  ///   declaration-seq declaration
  ///   declaration
  Declaration_seq
  Module_parser::parse_declaration_seq()
  {
    Declaration_seq ds;
    while (peek()) {
      Declaration* d = parse_declaration();
      ds.push_back(d);
    }
    return ds;
  }

  /// declaration:
  ///   function-definition
  ///   variable-definition
  Declaration* 
  Module_parser::parse_declaration()
  {
    switch (lookahead()) {
    case Token::func_kw:
      return parse_function_definition();
    case Token::val_kw:
    case Token::var_kw:
    case Token::ref_kw:
      return parse_data_definition();
    default:
      break;
    }

    std::stringstream ss;
    ss << "expected declaration, but got '" << peek() << "'\n";
    throw std::runtime_error(ss.str());
  }

  static bool
  is_data_head(const Token& tok)
  {
    return tok.is(Token::val_kw) 
        || tok.is(Token::var_kw) 
        || tok.is(Token::ref_kw);
  }

  /// data-definition:
  ///   data-head identifier type-clause initializer ';'
  ///   data-head identifier type-clause ';'
  ///   data-head identifier initializer ';'
  ///
  /// data-head:
  ///   'val' | 'var'
  Declaration*
  Module_parser::parse_data_definition()
  {
    assert(is_data_head(peek()));
    Token kw = consume();

    // Match the declaration name.
    Token id = match(Token::identifier);

    // Point of identification.
    Declaration* data = m_act.on_data_identification(id, kw);
    
    // Consume the tokens denoting the type.
    Token_seq type = consume_to(Token::equal);
    defer_data_type(data, std::move(type));

    // Consume the initializer and trailing semicolon.
    Token_seq init = consume_thru(Token::semicolon);
    defer_data_initializer(data, std::move(init));

    return nullptr;
  }


  /// function-definition:
  ///   'func' identifier function-signature function-body
  Declaration*
  Module_parser::parse_function_definition()
  {
    Token kw = require(Token::func_kw);

    // Match the function name.
    Token id = match(Token::identifier);

    // Point of identification.
    Declaration* fn = m_act.on_function_identification(id, kw);

    // Consume up to the opening brace.
    Token_seq sig = consume_to(Token::lbrace);
    defer_function_signature(fn, std::move(sig));

    // Consume the definition.
    Token_seq def = consume_thru(Token::rbrace);
    defer_function_definition(fn, std::move(def));

    return fn;
  }

  void
  Module_parser::defer_data_type(Declaration* d, Token_seq&& toks)
  {
    m_deferred_decls.emplace(new Deferred_data_type(m_cxt, d, std::move(toks)));
  }

  void
  Module_parser::defer_data_initializer(Declaration* d, Token_seq&& toks)
  {
    m_deferred_defs.emplace(new Deferred_data_initializer(m_cxt, d, std::move(toks)));
  }

  void
  Module_parser::defer_function_signature(Declaration* d, Token_seq&& toks)
  {
    m_deferred_decls.emplace(new Deferred_function_signature(m_cxt, d, std::move(toks)));
  }

  void
  Module_parser::defer_function_definition(Declaration* d, Token_seq&& toks)
  {
    m_deferred_defs.emplace(new Deferred_function_definition(m_cxt, d, std::move(toks)));
  }

  void
  Module_parser::parse_deferred_declarations()
  {
    parse_deferred_actions(m_deferred_decls);
  }
  
  void
  Module_parser::parse_deferred_definitions()
  {
    parse_deferred_actions(m_deferred_defs);
  }

  void
  Module_parser::parse_deferred_actions(std::queue<Deferred_parse*>& q)
  {
    while (!q.empty()) {
      Deferred_parse *p = q.front();
      q.pop();
      p->parse();
      delete p;
    }
  }

  void
  Deferred_data_type::parse()
  {
    Data_parser p(m_cxt);
    p.inject(m_toks);
    p.parse_deferred_data_type(m_decl);
  }

  void
  Deferred_data_initializer::parse()
  {
    Data_parser p(m_cxt);
    p.inject(m_toks);
    p.parse_deferred_data_initializer(m_decl);
  }

  void
  Deferred_function_signature::parse()
  {
    Function_parser p(m_cxt);
    p.inject(m_toks);
    p.parse_deferred_function_signature(m_decl);
  }

  void
  Deferred_function_definition::parse()
  {
    Function_parser p(m_cxt);
    p.inject(m_toks);
    p.parse_deferred_function_body(m_decl);
  }

} // namespace beaker