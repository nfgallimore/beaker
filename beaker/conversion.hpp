#pragma once

#include <beaker/expression.hpp>

namespace beaker
{
  /// Represents an implicit conversion from a source expression to a 
  /// destination type.
  class Conversion : public Unary_expression
  {
  public:
    enum Operation
    {
      to_value,
      to_bool,
      to_int,
      to_float,
    };
    
    Conversion(Type* t, Expression* e, Operation op)
      : Unary_expression(conv_kind, t, e), m_op(op)
    { }

    /// Returns the source expression.
    Expression* get_source() const { return get_operand(); }

    /// Returns the conversion operation.
    Operation get_operation() const { return m_op; }

    /// Returns the start location of the expression.
    Location get_start_location() const override;

    /// Returns the end location of the expression.
    Location get_end_location() const override;

  private:
    Operation m_op;
  };

  inline Location 
  Conversion::get_start_location() const
  { 
    return get_source()->get_start_location(); 
  }
  
  inline Location 
  Conversion::get_end_location() const  
  { 
    return get_source()->get_end_location(); 
  }


  /// Represents conversions of references to values.
  class Value_conversion : public Conversion
  {
  public:
    Value_conversion(Type* t, Expression* e)
      : Conversion(t, e, to_value)
    { }
  };


  /// Represents conversions to bool values.
  class Bool_conversion : public Conversion
  {
  public:
    Bool_conversion(Type* t, Expression* e)
      : Conversion(t, e, to_bool)
    { }
  };

} // namespace beaker
