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

      // arithmetic expressions
      add_kind,
      sub_kind,
      mul_kind,
      quo_kind,
      rem_kind,
      neg_kind,
      rec_kind,

      // bitwise expressions
      bit_and_kind,
      bit_ior_kind,
      bit_xor_kind,
      bit_not_kind,
      bit_shl_kind,
      bit_shr_kind,

      // logical expressions
      cond_kind,
      and_kind,
      or_kind,
      not_kind,

      // relational expressions
      eq_kind,
      ne_kind,
      lt_kind,
      gt_kind,
      ng_kind,
      nl_kind,

      // object expressions
      assign_kind,

      // conversions
      value_conv, // reference to value conversion
      bool_conv, // boolean conversion
      int_prom, // promotion of integer value to integer type
      sign_ext, // sign extension of integer type
      zero_ext, // zero extension of integer type
      int_trunc, // truncation of integer value
      float_prom, // promotion of integer value to fp type
      float_dem, // demotion of fp value to integer type
      float_ext, // extension of fp value to greater rank
      float_trunc, // truncation of fp value to lesser rank
      
      // initializers
      def_init, // default initialization
      val_init, // value initialization
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

    // Conversions

    /// Returns true if the expression is a narrowing conversion.
    bool is_narrowing_conversion() const;
    
    /// Returns true if the expression is a widening conversion.
    bool is_widening_conversion() const;

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


  /// The base class of all ternary expressions.
  class Ternary_expression : public Expression
  {
  protected:
    Ternary_expression(Kind k, Type* t, Expression* e1, Expression* e2, Expression* e3)
      : Expression(k, t), m_exprs{e1, e2, e3}
    { }

  public:
    /// Returns the first operand.
    Expression* get_first() const { return m_exprs[0]; }

    /// Returns the second operand.
    Expression* get_second() const { return m_exprs[1]; }

    /// Returns the third operand.
    Expression* get_third() const { return m_exprs[2]; }

  private:
    Expression* m_exprs[3];
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


  /// The base class of all ternary operator expressions. We assume that all
  /// such operators are in infix notation.
  class Ternary_operator : public Ternary_expression
  {
  protected:
    Ternary_operator(Kind k, 
                     Type* t, 
                     Expression* e1, 
                     Expression* e2, 
                     Expression* e3,
                     const Token& tok1,
                     const Token& tok2)
      : Ternary_expression(k, t, e1, e2, e3), m_toks{tok1, tok2}
    { }

  public:
    /// Returns the operator token (the first token).
    const Token& get_operator() const { return m_toks[0]; }

    /// Returns the separator token (the second toke).
    const Token& get_separator() const { return m_toks[1]; }

    /// Returns the start location of the expression. This is the start 
    /// location of the first operand.
    Location get_start_location() const override;
    
    /// Returns the end location of the expression. This is the end location
    /// of the third operand.
    Location get_end_location() const override;

  private:
    Token m_toks[2];
  };

  inline Location
  Ternary_operator::get_start_location() const
  {
    return get_first()->get_start_location();
  }
  
  inline Location
  Ternary_operator::get_end_location() const
  {
    return get_third()->get_end_location();
  }


  // Object expressions

  /// Represents the assignment of a value to an object.
  class Assignment_expression : public Binary_operator
  {
  public:
    Assignment_expression(Type* t, Expression* e1, Expression* e2, const Token& op)
      : Binary_operator(assign_kind, t, e1, e2, op)
    { }
  };

} // namespace beaker
