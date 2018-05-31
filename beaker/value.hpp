#pragma once

#include <beaker/common.hpp>

#include <cstdint>
#include <iosfwd>

namespace beaker
{
  class Typed_declaration;
  class Function_declaration;
  class Object;

  /// Represents an indeterminate value. Using these results in undefined
  /// behavior. These can be created as result of trivial initialization.
  struct Indeterminate_value { };

  /// The value representation of integer-typed expressions.
  using Int_value = std::intmax_t;

  /// The value representation of float-typed expressions.
  using Float_value = double;

  /// The value representation of function-typed expressions.
  using Function_value = const Function_declaration*;

  /// The value representation of reference-typed expressions.
  ///
  /// \todo This needs to be more than just an object pointer. We need to
  /// be able to ensure that every reference is to a valid object. Simply
  /// maintaining the address of the object will allow floating references,
  /// unless we keep a set of live objects.
  using Reference_value = Object*;


  /// Represents the computed value of an expression or held by an object.
  class Value
  {
    union Data
    {
      Indeterminate_value ind;
      Int_value z;
      Float_value fp;
      Function_value fn;
      Reference_value ref;
    };
  
  public:
    enum Kind 
    {
      indet_kind,
      int_kind,
      float_kind,
      func_kind,
      ref_kind,
    };

    explicit Value()
      : m_kind(indet_kind)
    { }

    explicit Value(Int_value n)
      : m_kind(int_kind)
    { m_data.z = n; }

    // Provided for convenience.
    explicit Value(int n)
      : m_kind(int_kind)
    { m_data.z = n; }

    explicit Value(Float_value n)
      : m_kind(float_kind)
    { m_data.fp = n; }

    explicit Value(Function_value f)
      : m_kind(func_kind)
    { m_data.fn = f; }

    explicit Value(Reference_value r)
      : m_kind(ref_kind)
    { m_data.ref = r; }

    // Kind

    /// Returns the kind of value.
    Kind get_kind() const { return m_kind; }

    /// Returns true if this is indeterminate.
    bool is_indeterminate() const { return m_kind == indet_kind; }

    /// Returns true if this is an integer.
    bool is_int() const { return m_kind == int_kind; }

    /// Returns true if this is a floating point value.
    bool is_float() const { return m_kind == float_kind; }

    /// Returns true if this is a function.
    bool is_function() const { return m_kind == func_kind; }

    /// Returns true if this is a reference.
    bool is_reference() const { return m_kind == ref_kind; }

    // Accessors
  
    /// Returns the underlying integer value.
    Int_value get_int() const;

    /// Returns the underlying floating point value.
    Float_value get_float() const;

    /// Returns the underlying function value.
    Function_value get_function() const;

    /// Returns the underlying reference value.
    Reference_value get_reference() const;

  private:
    /// The kind of value.
    Kind m_kind;

    /// Stores the specific value.
    Data m_data;
  };

  inline Int_value
  Value::get_int() const
  {
    assert(is_int());
    return m_data.z;
  }

  inline Float_value
  Value::get_float() const
  {
    assert(is_float());
    return m_data.fp;
  }

  inline Function_value
  Value::get_function() const
  {
    assert(is_function());
    return m_data.fn;
  }

  inline Reference_value
  Value::get_reference() const
  {
    assert(is_reference());
    return m_data.ref;
  }

  std::ostream& operator<<(std::ostream& os, const Value& v);

} // namespace beaker
