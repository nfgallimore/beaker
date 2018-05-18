#pragma once

#include <beaker/common.hpp>
#include <beaker/token.hpp>

namespace beaker
{
  /// The base class of all declarations.
  class Expression
  {
  public:
    enum Kind
    {
      bool_kind,
      int_kind,
      id_kind,

      and_kind,
      or_kind,
      not_kind,

      conv_kind,
    };

  protected:
    /// Constructs an expression of the given kind and type.
    Expression(Kind k, Type* t)
      : m_kind(k), m_type(t)
    { }

  public:
    virtual ~Expression() = default;

    // Kind

    /// Returns the kind of declaration.
    Kind get_kind() const { return m_kind; }

    /// Returns a string representation of the kind.
    const char* get_kind_name() const;

    // Type

    Type* get_type() const { return m_type; }

    // Location

    /// Returns the start location of the this statement.
    virtual Location get_start_location() const { return Location(); }
    
    /// Returns the end location of the this statement.
    virtual Location get_end_location() const { return Location(); }

    // Debugging

    void dump() const;

  private:
    /// The kind of declaration.
    Kind m_kind;

    /// The type of the expression.
    Type* m_type;
  };


  /// Represents literals comprised of a single token.
  class Literal : public Expression
  {
  protected:
    Literal(Kind k, Type* t, const Token& tok)
      : Expression(k, t), m_tok(tok)
    { }

  public:
    /// Returns the token for the literal.
    const Token& get_token() const { return m_tok; }

    /// Returns the start location of the this statement.
    Location get_start_location() const override { return m_tok.get_location(); }
    
    /// Returns the end location of the this statement.
    Location get_end_location() const override { return m_tok.get_location(); }

  private:
    Token m_tok;
  };


  /// Represents the boolean literals true and false.
  class Bool_literal : public Literal
  {
  public:
    Bool_literal(Type* t, const Token& tok, bool val)
      : Literal(bool_kind, t, tok), m_value(val)
    { }

    /// Returns the value of the literal.
    bool get_value() const { return m_value; }
  
  private:
    bool m_value;
  };


  /// Represents integer literals.
  class Int_literal : public Literal
  {
  public:
    Int_literal(Type* t, const Token& tok, bool val)
      : Literal(int_kind, t, tok), m_value(val)
    { }

    /// Returns the value of the literal.
    int get_value() const { return m_value; }
  
  private:
    int m_value;
  };


  /// Represents expressions that refer to declarations.
  class Id_expression : public Expression
  {
  public:
    Id_expression(Type* t, Typed_declaration* d)
      : Expression(id_kind, t), m_decl(d)
    { }

    /// Returns the declaration named by the expression.
    Typed_declaration* get_declaration() const { return m_decl; }
  
  private:
    Typed_declaration* m_decl;
  };

  /// The base class of all unary expressions.
  class Unary_expression : public Expression
  {
  protected:
    Unary_expression(Kind k, Type* t, Expression* e)
      : Expression(k, t), m_expr(e)
    { }

  public:
    /// Returns the operand of the expression.
    Expression* get_operand() const { return m_expr; }

  private:
    Expression* m_expr;
  };


  /// The base class of all binary expressions.
  class Binary_expression : public Expression
  {
  protected:
    Binary_expression(Kind k, Type* t, Expression* lhs, Expression* rhs)
      : Expression(k, t), m_exprs{lhs, rhs}
    { }

  public:
    /// Returns the left-hand operand.
    Expression* get_lhs() const { return m_exprs[0]; }

    /// Returns the right-hand operand.
    Expression* get_rhs() const { return m_exprs[1]; }

  private:
    Expression* m_exprs[2];
  };


  /// The base class of all unary operator expressions.
  class Unary_operator : public Unary_expression
  {
  protected:
    Unary_operator(Kind k, Type* t, Expression* e, const Token& op)
      : Unary_expression(k, t, e), m_op(op)
    { }

  public:
    /// Returns the operator token.
    const Token& get_operator() const { return m_op; }

    /// Returns the start location of the expression. This is the location
    /// of the operator token.
    Location get_start_location() const override;
    
    /// Returns the end location of the expression. This is the end location
    /// of the operand.
    Location get_end_location() const override;

  private:
    Token m_op;
  };

  inline Location
  Unary_operator::get_start_location() const
  {
    return m_op.get_location();
  }
  
  inline Location
  Unary_operator::get_end_location() const
  {
    return get_operand()->get_end_location();
  }


  /// The base class of all binary operator expressions. We assume that all
  /// such operators are in infix notation.
  class Binary_operator : public Binary_expression
  {
  protected:
    Binary_operator(Kind k, Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_expression(k, t, lhs, rhs), m_op(op)
    { }

  public:
    /// Returns the operator token.
    const Token& get_operator() const { return m_op; }

    /// Returns the start location of the expression. This is the start 
    /// location of the left-hand operand.
    Location get_start_location() const override;
    
    /// Returns the end location of the expression. This is the end location
    /// of the right-hand operand.
    Location get_end_location() const override;

  private:
    Token m_op;
  };

  inline Location
  Binary_operator::get_start_location() const
  {
    return get_lhs()->get_start_location();
  }
  
  inline Location
  Binary_operator::get_end_location() const
  {
    return get_rhs()->get_end_location();
  }

  // Arithmetic operators

  // Bitwise operators

  // Logical operators

  /// Represents logical and expressions.
  class Logical_and_expression : public Binary_operator
  {
  public:
    Logical_and_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(and_kind, t, lhs, rhs, op)
    { }
  };

  /// Represents logical and expressions.
  class Logical_or_expression : public Binary_operator
  {
  public:
    Logical_or_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(or_kind, t, lhs, rhs, op)
    { }
  };


  /// Represents logical not expressions.
  class Logical_not_expression : public Unary_operator
  {
  public:
    Logical_not_expression(Type* t, Expression* e, const Token& op)
      : Unary_operator(not_kind, t, e, op)
    { }
  };

} // namespace beaker
