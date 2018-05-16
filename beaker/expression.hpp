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
    };

  protected:
    /// Constructs a region of the given kind and with the associated region
    /// of source text.
    Expression(Kind k)
      : m_kind(k)
    { }

  public:
    virtual ~Expression() = default;

    // Kind

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


} // namespace beaker
