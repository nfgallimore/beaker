#pragma once

#include <beaker/common.hpp>
#include <beaker/token.hpp>

namespace beaker
{
  /// The base class of all declarations.
  class Statement
  {
  public:
    enum Kind
    {
      block_kind,
    };

  protected:
    /// Constructs a region of the given kind and with the associated region
    /// of source text.
    Statement(Kind k)
      : m_kind(k)
    { }

  public:
    virtual ~Statement();

    /// Returns the kind of declaration.
    Kind get_kind() const { return m_kind; }

    /// Returns the start location of the this statement.
    virtual Location get_start_location() const { return Location(); }
    
    /// Returns the end location of the this statement.
    virtual Location get_end_location() const { return Location(); }

  private:
    /// The kind of declaration.
    Kind m_kind;
  };


  /// A sequence of statements. 
  ///
  /// Note that a block statement has an associated declarative region. The
  /// declarations of declaration statements are declared within the associated
  /// region.
  class Block_statement : public Statement
  {
  public:
    Block_statement(const Token& lb, const Token& rb)
      : Statement(block_kind), m_braces{lb, rb}
    { }

    /// Returns the sequence of statements.
    const Statement_seq& get_statements() const { return m_stmts; }

    /// Adds a statement to the current block.
    void add_statement(Statement* s) { m_stmts.push_back(s); }

    /// Returns the start location of the this statement.
    Location get_start_location() const override { return m_braces[0].get_location(); }
    
    /// Returns the end location of the this statement.
    Location get_end_location() const override { return m_braces[1].get_location(); }
  
  private:
    /// The nested sequence of statements.
    Statement_seq m_stmts;

    /// The opening and closing braces.
    Token m_braces[2];
  };

} // namespace beaker
