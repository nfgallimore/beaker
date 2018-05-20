#include "semantics.hpp"
#include "scope.hpp"
#include "declaration.hpp"
#include "type.hpp"
#include "type_specifier.hpp"
#include "expression.hpp"
#include "statement.hpp"
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
    // Push a new block scope on the stack.
    m_scope = new Block_scope(s, m_scope);
  }

  void
  Semantics::leave_scope(Declaration* d)
  {
    assert(d->is_scoped());
    assert(m_decl == d->cast_as_scoped()); // Imbalanced stack

    // Pop the current scope.
    //
    // FIXME: Check that the scope also refers to d.
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
    // FIXME: Check that the scope refers to s.
    assert(m_scope); // Imbalanced stack
    
    Scope* prev = m_scope;
    m_scope = m_scope->get_parent();
    delete prev;
  }

  void
  Semantics::restore_scope(Declaration* d)
  {
    assert(!m_scope && !m_decl); // Must be in a pristine state.

    // Restore the scope up to, but not including d.
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
    d = d->get_enclosing_declaration();

    while (m_scope) {
      leave_scope(d);
      d = d->get_enclosing_declaration();
    }
  }

  Function_declaration*
  Semantics::get_current_function()
  {
    Declaration* d = m_decl->cast_as_declaration();
    if (auto* fn = dynamic_cast<Function_declaration*>(d))
      return fn;
    return nullptr;
  }

  Block_statement*
  Semantics::get_current_block()
  {
    if (auto* bs = dynamic_cast<Block_scope*>(m_scope))
      return static_cast<Block_statement*>(bs->get_statement());
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
