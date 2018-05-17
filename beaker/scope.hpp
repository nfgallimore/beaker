#pragma once

#include "declaration.hpp"

namespace beaker
{
  /// The base class of scope objects. Scopes can be derived in order to
  /// provide different lookup facilities.
  class Scope
  {
  protected:
    Scope(Scope* p)
      : m_parent(p)
    { }

  public:
    virtual ~Scope() = default;

    /// Returns the parent scope.
    Scope* get_parent() const { return m_parent; }

    /// Search this scope for a declaration.
    virtual Declaration_set lookup(Symbol sym) const = 0;

    /// Registers the declaration for name lookup in this scope.
    virtual void declare(Named_declaration* d) = 0;

  private:
    /// The parent scope.
    Scope* m_parent;
  };


  /// Represents scopes associated a declaration.
  class Declaration_scope : public Scope
  {
  public:
    Declaration_scope(Scoped_declaration* d, Scope* p)
      : Scope(p), m_decl(d)
    { }

    /// Returns the associated declaratin.
    Scoped_declaration* get_declaration() const { return m_decl; }

    /// Search this scope for a declaration.
    Declaration_set lookup(Symbol sym) const override;

    /// Registers the declaration for name lookup in this scope.
    void declare(Named_declaration* d) override;

  private:
    Scoped_declaration* m_decl;
  };

  inline Declaration_set 
  Declaration_scope::lookup(Symbol sym) const
  { 
    return m_decl->lookup(sym); 
  }

  inline void 
  Declaration_scope::declare(Named_declaration* d)
  { 
    m_decl->add_visible_declaration(d); 
  }


  /// Represents scopes associated with a statement.
  class Block_scope : public Scope
  {
  public:
    Block_scope(Statement* s, Scope* p)
      : Scope(p), m_stmt(s)
    { }

    /// Returns the underlying statement
    Statement* get_statement() const;

    /// Search this scope for a declaration.
    Declaration_set lookup(Symbol sym) const override;

    /// Registers the declaration for name lookup in this scope.
    void declare(Named_declaration* d) override;

  private:
    /// The associated statement.
    Statement* m_stmt;

    /// The set of declarations in this scope.
    Declaration_map m_lookup;
  };

  inline Declaration_set 
  Block_scope::lookup(Symbol sym) const 
  { 
    return m_lookup.lookup(sym); 
  }
  
  void 
  Block_scope::declare(Named_declaration* d) 
  { 
    m_lookup.declare(d); 
  }

} // namespace beaker
