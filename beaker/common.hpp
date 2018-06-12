#pragma once

#include <cassert>
#include <memory>
#include <vector>

namespace beaker
{
  /// Represents the radix (or base) of numeric literals.
  enum class Radix
  {
    binary = 2,
    decimal = 10,
    hexadecimal = 16,
  };

  /// Represents the available encoding of text literals.
  enum class Encoding
  {
    basic,
    utf8,
    utf16,
    utf32,
  };

  // Declarations
  
  class Location;
  class Context;
  class Type;
  class Type_specifier;
 
  class Expression;
  class Bool_literal;
  class Int_literal;
  class Id_expression;
  class Conditional_expression;
  class Logical_and_expression;
  class Logical_or_expression;
  class Logical_not_expression;
  class Assignment_expression;

  class Conversion;
  class Implicit_conversion;

  class Initializer;
  class Empty_initializer;
  class Default_initializer;
  class Value_initializer;
 
  class Statement;
  class Block_statement;
  class If_statement;
  class While_statement;
  class Break_statement;
  class Continue_statement;
  class Return_statement;
  class Expression_statement;
  class Declaration_statement;
  class Condition;
  
  class Declaration;
  class Scoped_declaration;
  class Named_declaration;
  class Typed_declaration;
  class Data_declaration;
  class Variable_declaration;
  class Value_declaration;
  class Reference_declaration;
  class Function_declaration;
  
  class Parameter;

  class Value;
  class Object;

  /// A sequence of types.
  using Type_seq = std::vector<Type*>;

  /// A sequence of type specifiers.
  using Type_specifier_seq = std::vector<Type_specifier*>;
  
  /// A sequence of expressions.
  using Expression_seq = std::vector<Expression*>;

  /// A pair fo expressions.
  using Expression_pair = std::pair<Expression*, Expression*>;
  
  /// A sequence of statements.
  using Statement_seq = std::vector<Statement*>;
  
  /// A sequence of declarations.
  using Declaration_seq = std::vector<Declaration*>;

  /// A sequence of declarations.
  using Parameter_seq = std::vector<Parameter*>;


  /// A syntactic construct is a type specifier, expression, statement,
  /// or declaration. 
  class Construct
  {
    union Data
    {
      Type_specifier* t;
      Expression* e;
      Statement* s;
      Declaration* d;
    };
  public:
    /// Kinds of syntactic constructs.
    ///
    /// \note These must match the order of types in the Construct variant
    /// in the base class.
    enum Kind
    {
      type_kind,
      expr_kind,
      stmt_kind,
      decl_kind,
    };

    Construct(Type_specifier* t)
      : m_kind(type_kind)
    { m_data.t = t; }

    Construct(Expression* e)
      : m_kind(expr_kind)
    { m_data.e = e; }

    Construct(Statement* s)
      : m_kind(stmt_kind)
    { m_data.s = s; }

    Construct(Declaration* d)
      : m_kind(decl_kind)
    { m_data.d = d; }
  
    // Queries

    /// Returns the kind of construct.
    Kind get_kind() const { return m_kind; }

    /// Returns true if this is a type specifier.
    bool is_type() const { return get_kind() == type_kind; }

    /// Returns true if this is an expression.
    bool is_expression() const { return get_kind() == expr_kind; }

    /// Returns true if this is a statement.
    bool is_statement() const { return get_kind() == stmt_kind; }

    /// Returns true if this is a declaration.
    bool is_declaration() const { return get_kind() == decl_kind; }

    // Access

    /// Returns this as a type specifier.
    Type_specifier* get_type() const;

    /// Returns this as an expression.
    Expression* get_expression() const;

    /// Returns this as a statement.
    Statement* get_statement() const;

    /// Returns this as a declaration.
    Declaration* get_declaration() const;

  private:
    Kind m_kind;
    Data m_data;
  };

  inline Type_specifier*
  Construct::get_type() const
  {
    assert(is_type()); return m_data.t;
  }

  inline Expression*
  Construct::get_expression() const
  {
    assert(is_expression()); return m_data.e;
  }

  inline Statement*
  Construct::get_statement() const
  {
    assert(is_statement()); return m_data.s;
  }

  inline Declaration*
  Construct::get_declaration() const
  {
    assert(is_declaration()); return m_data.d;
  }


} // namespace beaker
