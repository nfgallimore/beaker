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
      when_kind,
      if_kind,
      while_kind,
      break_kind,
      cont_kind,
      ret_kind,
      expr_kind,
      decl_kind,
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

    // Debugging

    /// Emit a textual representation of the statement.
    void dump() const;

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

    /// Set the statements for the block scope.
    void set_statements(const Statement_seq& ss) { m_stmts = ss; }

    /// Set the statements for the block scope.
    void set_statements(Statement_seq&& ss) { m_stmts = std::move(ss); }

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


  /// Represents statements of the form `if (e) s`.
  class When_statement : public Statement
  {
  public:
    When_statement(Expression* e, 
                   Statement* s, 
                   Location start, 
                   Location lp, 
                   Location rp)
      : Statement(when_kind), m_cond(e), m_true(s), m_locs{start, lp, rp}
    { }

    /// Returns the condition.
    Expression* get_condition() const { return m_cond; }

    /// Return the branch to execute when the condition is true.
    Statement* get_true_branch() const { return m_true; }

  private:
    /// The condition.
    Expression* m_cond;

    /// The statement to execute when the condition is true.
    Statement* m_true;

    /// The locations of 'if', '(', and ')', respectively.
    Location m_locs[3];
  };


  /// Represents statements of the form `if (e) s1 else s2`.
  class If_statement : public Statement
  {
  public:
    If_statement(Expression* e, 
                 Statement* s1, 
                 Statement* s2, 
                 Location start, 
                 Location lp, 
                 Location rp,
                 Location el)
      : Statement(if_kind), m_cond(e), m_true(s1), m_false(s2), m_locs{start, lp, rp, el}
    { }

    /// Returns the condition.
    Expression* get_condition() const { return m_cond; }

    /// Return the branch to execute when the condition is true.
    Statement* get_true_branch() const { return m_true; }

    /// Return the branch to execute when the condition is false.
    Statement* get_false_branch() const { return m_false; }

  private:
    /// The condition.
    Expression* m_cond;

    /// The statement to execute when the condition is true.
    Statement* m_true;

    /// The statement to execute when the condition is false.
    Statement* m_false;

    /// The locations of 'if', '(', ')', and 'else' respectively.
    Location m_locs[5];
  };


  /// Represents statements of the form `if (e) s`.
  class While_statement : public Statement
  {
  public:
    While_statement(Expression* e, 
                    Statement* s, 
                    Location start, 
                    Location lp, 
                    Location rp)
      : Statement(while_kind), m_cond(e), m_body(s), m_locs{start, lp, rp}
    { }

    /// Returns the condition.
    Expression* get_condition() const { return m_cond; }

    /// Returns the loop body.
    Statement* get_body() const { return m_body; }

  private:
    /// The condition.
    Expression* m_cond;

    /// The loop body.
    Statement* m_body;

    /// The locations of 'while', '(', and ')', respectively.
    Location m_locs[3];
  };


  /// Represents statements of the form `break;`.
  class Break_statement : public Statement
  {
  public:
    Break_statement(Location kw, Location semi)
      : Statement(break_kind), m_locs{kw, semi}
    { }

  private:
    /// The 'break' and ';' locations, respectively.
    Location m_locs[2];
  };


  /// Represents statements of the form `continue;`.
  class Continue_statement : public Statement
  {
  public:
    Continue_statement(Location kw, Location semi)
      : Statement(cont_kind), m_locs{kw, semi}
    { }

  private:
    /// The 'break' and ';' locations, respectively.
    Location m_locs[2];
  };


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


  /// Reprsents a statement that declares a local entity.
  class Declaration_statement : public Statement
  {
  public:
    Declaration_statement(Declaration* d)
      : Statement(decl_kind), m_decl(d)
    { }

    /// Returns the declaration.
    Declaration* get_declaration() const { return m_decl; }

  private:
    Declaration* m_decl;
  };

} // namespace beaker
