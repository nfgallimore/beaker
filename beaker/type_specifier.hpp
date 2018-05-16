#pragma once

#include <beaker/common.hpp>
#include <beaker/token.hpp>

namespace beaker
{
  /// The base class of all type specifiers.
  class Type_specifier
  {
  public:
    enum Kind
    {
      simple_kind,
    };
  
  protected:
    /// Constructs a type specifier of the given kind.
    Type_specifier(Kind k, Type* t)
      : m_kind(k), m_type(t)
    { }

  public:
    virtual ~Type_specifier() = default;

    // Kind

    /// Returns the kind of type specifier.
    Kind get_kind() const { return m_kind; }

    // Physical location

    /// Returns the start location of the this statement.
    virtual Location get_start_location() const { return Location(); }
    
    /// Returns the end location of the this statement.
    virtual Location get_end_location() const { return Location(); }

    // Canonical type

    /// Returns the type denoted by the type specifier.
    Type* get_type() const { return m_type; }

  private:
    /// The kind of type specifier.
    Kind m_kind;

    /// The type denoted by the specifier.
    Type* m_type;
  };


  /// Represents a simple type specifier.
  class Simple_type_specifier : public Type_specifier
  {
  public:
    Simple_type_specifier(const Token& tok, Type* t)
      : Type_specifier(simple_kind, t)
    { }

    /// Returns the token for the type specifier.
    const Token& get_token() const { return m_tok; }

  private:
    /// The token representing the specifier.
    Token m_tok;
  };

} // namespace beaker
