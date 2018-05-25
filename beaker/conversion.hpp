#pragma once

#include <beaker/expression.hpp>

namespace beaker
{
  /// The base class of all conversions, implicit and explict.
  ///
  /// \note For explicit conversion, see Cast.
  class Conversion : public Unary_expression
  {
  public:
    enum Conversion_kind
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

  protected:
    Conversion(Kind k, Conversion_kind ck, Type* t, Expression* e)
      : Unary_expression(k, t, e), m_conv(ck)
    { }

  public:
    /// Returns the source expression.
    Expression* get_source() const { return get_operand(); }

    /// Returns the conversion kind.
    Conversion_kind get_conversion_kind() const { return m_conv; }

    /// Returns a textual representation of the conversion name.
    const char* get_conversion_name() const;

    /// Returns true if the expression is a narrowing conversion.
    bool is_narrowing_conversion() const;
    
    /// Returns true if the expression is a widening conversion.
    bool is_widening_conversion() const;

  private:
    /// The kind of conversion performed.
    Conversion_kind m_conv;
  };


  /// Represents an implicit conversion, which is not expressed directly
  /// in the source code. Implicit conversions are created as a result of
  /// semantic analysis. For example:
  ///
  ///     var x : int = 42;
  ///     val y = a; // Reference-to-value conversion
  ///
  /// The binding of y to the value of a requires an implicit reference to
  /// value conversion.
  class Implicit_conversion : public Conversion
  {
  public:
    Implicit_conversion(Conversion_kind ck, Type* t, Expression* e)
      : Conversion(imp_conv, ck, t, e)
    { }

    /// Returns the start location of the expression.
    Location get_start_location() const override;

    /// Returns the end location of the expression.
    Location get_end_location() const override;
  };

  inline Location 
  Implicit_conversion::get_start_location() const
  { 
    return get_source()->get_start_location(); 
  }
  
  inline Location 
  Implicit_conversion::get_end_location() const  
  { 
    return get_source()->get_end_location(); 
  }

} // namespace beaker
