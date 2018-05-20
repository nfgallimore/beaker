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
      ret_kind,
      expr_kind,
    };

  protected:
    /// Constructs a region of the given kind and with the associated region
    /// of source text.
    Statement(Kind k)
      : m_kind(k)
    { }

  public:
    virtual ~Statement() = default;

    // Kind

    /// Returns the kind of declaration.
    Kind get_kind() const { return m_kind; }

    /// Returns a textual representation of the kind.
    const char* get_kind_name() const;

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
    Block_statement()
      : Statement(block_kind), m_braces{}
    { }

    /// Returns the sequence of statements.
    const Statement_seq& get_statements() const { return m_stmts; }

    /// Adds a statement to the current block.
    void add_statement(Statement* s) { m_stmts.push_back(s); }

    /// Returns the start location of the this statement.
    Location get_start_location() const override { return m_braces[0]; }
    
    /// Returns the end location of the this statement.
    Location get_end_location() const override { return m_braces[1]; }

    /// Sets the locations of the left and right brace. 
    void set_brace_locations(Location lbrace, Location rbrace);
  
  private:
    /// The nested sequence of statements.
    Statement_seq m_stmts;

    /// The opening and closing braces.
    Location m_braces[2];
  };

  inline void
  Block_statement::set_brace_locations(Location lbrace, Location rbrace)
  {
    m_braces[0] = lbrace;
    m_braces[1] = rbrace;
  }


  /// Represents return statements.
  class Return_statement : public Statement
  {
  public:
    Return_statement(Expression* e, Location kw, Location semi)
      : Statement(ret_kind), m_expr(e), m_locs{kw, semi}
    { }

    /// Returns the expression.
    Expression* get_return_value() const { return m_expr; }

  private:
    /// The computed return value.
    Expression* m_expr;

    /// The location of `return`  and ';', respectively.
    Location m_locs[2];
  };


  /// Represents expression statements.
  class Expression_statement : public Statement
  {
  public:
    Expression_statement(Expression* e, Location semi)
      : Statement(expr_kind), m_expr(e), m_loc(semi)
    { }

    /// Returns the expression.
    Expression* get_expression() const { return m_expr; }

  private:
    /// The computed return value.
    Expression* m_expr;

    /// The location of the `;`.
    Location m_loc;
  };

} // namespace beaker
