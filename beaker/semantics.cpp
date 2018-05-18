#include "semantics.hpp"
#include "scope.hpp"
#include "declaration.hpp"
#include "type.hpp"
#include "type_specifier.hpp"
#include "expression.hpp"
#include "logical_expression.hpp"
#include "conversion.hpp"
#include "context.hpp"
#include "print.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

namespace beaker
{
  Semantics::Semantics(Context& cxt)
    : m_cxt(cxt), m_scope(), m_decl()
  { }

  Semantics::~Semantics()
  {
    assert(!m_scope); // Imbalanced scope stack
    assert(!m_decl); // Imbalanced declaration stack
  }

  void
  Semantics::enter_scope(Declaration* d)
  {
    assert(d->is_scoped());
    assert(m_decl != d->cast_as_scoped()); // Already on the stack.
    
    // Make d the current declaration.
    Scoped_declaration* sd = d->cast_as_scoped();
    m_decl = sd;

    // Push a new scope for the declaration.
    m_scope = new Declaration_scope(sd, m_scope);
  }

  void
  Semantics::enter_scope(Statement* s)
  {
    __builtin_unreachable();
  }

  void
  Semantics::leave_scope(Declaration* d)
  {
    assert(d->is_scoped());
    assert(m_decl == d->cast_as_scoped()); // Imbalanced stack

    // Pop the current scope.
    Scope* prev = m_scope;
    m_scope = m_scope->get_parent();
    delete prev;

    // Make d's owner the current scope.
    //
    // Equivalently, this should be declaration associated with
    // the current scope.
    m_decl = d->get_owner();
  }

  void
  Semantics::leave_scope(Statement* s)
  {
    __builtin_unreachable();
  }

  void
  Semantics::restore_scope(Declaration* d)
  {
    assert(!m_scope && !m_decl); // Must be in a pristine state.

    // Restore the scope up to, but not including d.
    if (!d->is_scoped())
      d = d->get_enclosing_declaration();

    // Compute the enclosing scopes of d.
    Declaration_seq decls;
    while (d) {
      decls.push_back(d);
      d = d->get_enclosing_declaration();
    }
    std::reverse(decls.begin(), decls.end());
    
    // Reconstruct the stack.
    for (Declaration* d : decls)
      enter_scope(d);
  }

  void
  Semantics::empty_scope(Declaration* d)
  {
    // Restore the scope up to, but not including d.
    if (!d->is_scoped())
      d = d->get_enclosing_declaration();

    while (m_scope) {
      leave_scope(d);
      d = d->get_enclosing_declaration();
    }
  }

  Type_specifier*
  Semantics::on_unit_type(const Token& tok)
  {
    return new Simple_type_specifier(tok, m_cxt.get_unit_type());
  }
  
  Type_specifier*
  Semantics::on_bool_type(const Token& tok)
  {
    return new Simple_type_specifier(tok, m_cxt.get_bool_type());
  }
  
  Type_specifier*
  Semantics::on_int_type(const Token& tok)
  {
    return new Simple_type_specifier(tok, m_cxt.get_int_type());
  }
  
  Type_specifier*
  Semantics::on_float_type(const Token& tok)
  {
    return new Simple_type_specifier(tok, m_cxt.get_float_type());
  }
  
  Type_specifier*
  Semantics::on_char_type(const Token& tok)
  {
    __builtin_unreachable();
  }

  Statement*
  Semantics::on_block_statement(const Statement_seq& stmts,
                                const Token& lbrace,
                                const Token& rbrace)
  {
    return nullptr;
  }

  Statement*
  Semantics::on_when_statement(Expression* e, 
                               Statement* s, 
                               const Token& kw,
                               const Token& lparen,
                               const Token& rparen)
  {
    return nullptr;
  }

  Statement*
  Semantics::on_if_statement(Expression* e, 
                             Statement* s1, 
                             Statement* s2,
                             const Token& kw1,
                             const Token& lparen,
                             const Token& rparen,
                             const Token& kw2)
  {
    return nullptr;
  }

  Statement*
  Semantics::on_while_statement(Expression* e, 
                                Statement* s,
                                const Token& kw,
                                const Token& lparen,
                                const Token& rparen)
  {
    return nullptr;
  }

  Statement*
  Semantics::on_break_statement(const Token& kw, 
                                const Token& semi)
  {
    return nullptr;
  }

  Statement*
  Semantics::on_continue_statement(const Token& kw,
                                   const Token& semi)
  {
    return nullptr;
  }

  Statement*
  Semantics::on_return_statement(Expression* e,
                                 const Token& kw,
                                 const Token& semi)
  {
    return nullptr;
  }

  Statement*
  Semantics::on_declaration_statement(Declaration* d)
  {
    return nullptr;
  }

  Statement*
  Semantics::on_expression_statement(Expression* e, 
                                     const Token& semi)
  {
    return nullptr;
  }

  // Conditions

  Condition* 
  Semantics::on_condition(Expression* e)
  {
    return nullptr;
  }

  // Lookup

  Declaration_set
  Semantics::unqualified_lookup(Symbol sym)
  {
    Scope* s = m_scope;
    while (s) {
      Declaration_set decls = s->lookup(sym);
      if (!decls.is_empty())
        return decls;
      s = s->get_parent();
    }
    return {};
  }


} // namespace beaker
