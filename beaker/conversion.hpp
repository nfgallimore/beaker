#pragma once

#include <beaker/expression.hpp>

namespace beaker
{
  /// Represents an implicit conversion from a source expression to a 
  /// destination type.
  class Conversion : public Unary_expression
  {
  protected:
    Conversion(Kind k, Type* t, Expression* e)
      : Unary_expression(k, t, e)
    { }

  public:
    /// Returns the source expression.
    Expression* get_source() const { return get_operand(); }

    /// Returns a textual representation of the conversion name.
    const char* get_conversion_name() const;

    /// Returns the start location of the expression.
    Location get_start_location() const override;

    /// Returns the end location of the expression.
    Location get_end_location() const override;
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
      : Conversion(value_conv, t, e)
    { }
  };


  /// Represents conversions to bool values.
  class Bool_conversion : public Conversion
  {
  public:
    Bool_conversion(Type* t, Expression* e)
      : Conversion(bool_conv, t, e)
    { }
  };


  /// Represents the promotion of integer values to an integer type.
  class Int_promotion : public Conversion
  {
  public:
    Int_promotion(Type* t, Expression* e)
      : Conversion(int_prom, t, e)
    { }
  };


  /// Represents the sign extension of integer values to a greater rank.
  class Sign_extension : public Conversion
  {
  public:
    Sign_extension(Type* t, Expression* e)
      : Conversion(sign_ext, t, e)
    { }
  };


  /// Represents the zero extension of integer values to a greater rank.
  class Zero_extension : public Conversion
  {
  public:
    Zero_extension(Type* t, Expression* e)
      : Conversion(zero_ext, t, e)
    { }
  };


  /// Represents the truncation of an integer value to a lower rank.
  class Int_truncation : public Conversion
  {
  public:
    Int_truncation(Type* t, Expression* e)
      : Conversion(int_trunc, t, e)
    { }
  };


  /// Represents the conversion of integer values to a floating point type.
  class Float_promotion : public Conversion
  {
  public:
    Float_promotion(Type* t, Expression* e)
      : Conversion(float_prom, t, e)
    { }
  };


  /// Represents the conversion of floating point values to an integer type.
  class Float_demotion : public Conversion
  {
  public:
    Float_demotion(Type* t, Expression* e)
      : Conversion(float_dem, t, e)
    { }
  };


  /// Represets the extension of floating point values to a greater rank.
  class Float_extension : public Conversion
  {
  public:
    Float_extension(Type* t, Expression* e)
      : Conversion(float_ext, t, e)
    { }
  };

  /// Represets the truncation of floating point values to a lesser rank.
  class Float_truncation : public Conversion
  {
  public:
    Float_truncation(Type* t, Expression* e)
      : Conversion(float_trunc, t, e)
    { }
  };

} // namespace beaker
