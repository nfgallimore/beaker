#pragma once

#include <memory>
#include <variant>
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
  class Conversion;
  class Initializer;
  class Condition;
  class Statement;
  class Declaration;
  class Scoped_declaration;
  class Named_declaration;
  class Typed_declaration;
  class Parameter;

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
  class Construct : public std::variant<Type_specifier*, Expression*, Statement*, Declaration*>
  {
    using Base = std::variant<Type_specifier*, Expression*, Statement*, Declaration*>;
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

    using Base::variant;
  
    // Queries

    /// Returns the kind of construct.
    Kind get_kind() const { return static_cast<Kind>(index()); }

    /// Returns true if this is a type specifier.
    bool is_type_specifier() const { return get_kind() == type_kind; }

    /// Returns true if this is an expression.
    bool is_expression() const { return get_kind() == expr_kind; }

    /// Returns true if this is a statement.
    bool is_statement() const { return get_kind() == stmt_kind; }

    /// Returns true if this is a declaration.
    bool is_declaration() const { return get_kind() == decl_kind; }

    // Variant access

    /// Returns this as a type specifier.
    Type_specifier* get_type() const { return std::get<Type_specifier*>(*this); }

    /// Returns this as an expression.
    Expression* get_expression() const { return std::get<Expression*>(*this); }

    /// Returns this as a statement.
    Statement* get_statement() const { return std::get<Statement*>(*this); }

    /// Returns this as a declaration.
    Declaration* get_declaration() const { return std::get<Declaration*>(*this); }
  };

} // namespace beaker
