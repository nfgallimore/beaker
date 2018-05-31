#pragma once

#include <beaker/expression.hpp>

namespace beaker
{
  /// Represents an object initializer. Note that initializers are untyped; 
  /// they do not produce values. An initializer is executed to supply
  /// an object with a value.
  class Initializer : public Expression
  {
  protected:
    Initializer(Kind k, Expression* obj)
      : Expression(k, nullptr), m_obj(obj)
    { }

  public:
    /// Returns the object being initialized. For variable declarations,
    /// this is an id-expression referring to the object. For dynamic
    /// allocations and temporaries, this is the expression that creates
    /// the object. Note that this must be a reference.
    Expression* get_object() const { return m_obj; }

    /// Returns the declared object being initialized. Returns null if
    /// the initializer is applied to an object created dynamically.
    Typed_declaration* get_declaration() const;

  private:
    Expression* m_obj;
  };

  
  /// The empty initializer applies implicit default initialization. This 
  /// corresponds to declarations of the form:
  ///
  ///     var x : int;
  ///
  /// Implicit default initialization trivially initializes certain objects.
  class Empty_initializer : public Initializer
  {
  public:
    Empty_initializer(Expression* obj)
      : Initializer(empty_init, obj)
    { }
  };


  /// The empty initializer applies explicit default initialization. This 
  /// corresponds to declarations of the form:
  ///
  ///     var x : int = default;
  ///
  /// Explicit default initialization zero-initializes certain objects.
  class Default_initializer : public Initializer
  {
  public:
    Default_initializer(Expression* obj)
      : Initializer(def_init, obj)
    { }
  };


  /// The value initializer applies copy initialization. This corresponds to 
  /// declarations of the form:
  ///
  ///     var x : int = e;
  ///
  /// The value of e is stored in the variable `x`.
  class Value_initializer : public Initializer
  {
  public:
    Value_initializer(Expression* obj, Expression* e)
      : Initializer(val_init, obj), m_expr(e)
    { }

    /// Returns the value used for initialization.
    Expression* get_value() const { return m_expr; }

  private:
    Expression* m_expr;
  };

} // namespace beaker
