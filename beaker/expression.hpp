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


} // namespace beaker
