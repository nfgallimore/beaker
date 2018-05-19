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
      : Conversion(t, e, value_conv)
    { }
  };


  /// Represents conversions to bool values.
  class Bool_conversion : public Conversion
  {
  public:
    Bool_conversion(Type* t, Expression* e)
      : Conversion(t, e, bool_conv)
    { }
  };


  /// Represents the promotion of integer values to an integer type.
  class Int_promotion : public Conversion
  {
  public:
    Int_promotion(Type* t, Expression* e)
      : Conversion(t, e, int_prom)
    { }
  };


  /// Represents the sign extension of integer values to a greater rank.
  class Sign_extension : public Conversion
  {
  public:
    Sign_extension(Type* t, Expression* e)
      : Conversion(t, e, sign_ext)
    { }
  };


  /// Represents the zero extension of integer values to a greater rank.
  class Zero_extension : public Conversion
  {
  public:
    Zero_extension(Type* t, Expression* e)
      : Conversion(t, e, zero_ext)
    { }
  };


  /// Represents the truncation of an integer value to a lower rank.
  class Int_truncation : public Conversion
  {
  public:
    Int_truncation(Type* t, Expression* e)
      : Conversion(t, e, int_trunc)
    { }
  };


  /// Represents the conversion of integer values to a floating point type.
  class Float_promotion : public Conversion
  {
  public:
    Float_promotion(Type* t, Expression* e)
      : Conversion(t, e, float_prom)
    { }
  };


  /// Represents the conversion of floating point values to an integer type.
  class Float_demotion : public Conversion
  {
  public:
    Float_demotion(Type* t, Expression* e)
      : Conversion(t, e, float_dem)
    { }
  };


  /// Represets the extension of floating point values to a greater rank.
  class Float_extension : public Conversion
  {
  public:
    Float_extension(Type* t, Expression* e)
      : Conversion(t, e, float_ext)
    { }
  };

  /// Represets the truncation of floating point values to a lesser rank.
  class Float_truncation : public Conversion
  {
  public:
    Float_truncation(Type* t, Expression* e)
      : Conversion(t, e, float_trunc)
    { }
  };

} // namespace beaker
