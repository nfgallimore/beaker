#pragma once

#include <beaker/expression.hpp>

namespace beaker
{
  /// Represents a data initializer. Note that initializers are untyped; they
  /// do not produce values.
  ///
  /// \todo 
  class Initializer : public Expression
  {
  protected:
    Initializer(Kind k)
      : Expression(k, nullptr)
    { }
  };

  
  /// A default initializer applies default initialization to an object.
  class Default_initializer : public Initializer
  {
  public:
    Default_initializer()
      : Initializer(def_init)
    { }
  };

  /// A value initializer supplies a value to the entity initialized. If
  /// the entity is an object, the object receives a copy ofthe value.
  class Value_initializer : public Initializer
  {
  public:
    Value_initializer(Expression* e)
      : Initializer(val_init), m_expr(e)
    { }

    Expression* get_value() const { return m_expr; }

  private:
    Expression* m_expr;
  };

} // namespace beaker
