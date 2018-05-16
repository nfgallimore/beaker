#pragma once

#include <beaker/common.hpp>

namespace beaker
{
  /// Represents the set of types. Types are entities; they have no syntactic
  /// representation. 
  class Type
  {
  public:
    /// Kinds of types.
    enum Kind {
      unit_kind,
      bool_kind,
      int_kind,
      float_kind,
      auto_kind,
      func_kind,
      ref_kind,
    };

  protected:
    /// Constructs a type with the given kind.
    Type(Kind k) : m_kind(k) { }

  public:
    /// Returns the kind of type.
    Kind get_kind() const { return m_kind; }

    /// Returns true if this is the type `unit`.
    bool is_unit() const { return m_kind == unit_kind; }

    /// Returns true if this is the type `bool`.
    bool is_bool() const { return m_kind == bool_kind; }

    /// Returns true if this is one of the integer types.
    bool is_integer() const { return m_kind == int_kind; }
    
    /// Returns true if this is one of the floating point types.
    bool is_floating_point() const { return m_kind == float_kind; }

    /// Returns true if this is the type `auto`.
    bool is_auto() const { return m_kind == auto_kind; }
  
  private:
    Kind m_kind;
  };


  /// Represents the singleton type `unit`.
  class Unit_type : public Type
  {
  public:
    Unit_type() : Type(unit_kind) { }
  };


  /// Represents the singleton type `bool`.
  class Bool_type : public Type
  {
  public:
    Bool_type() : Type(bool_kind) { }
  };


  /// Represents the types `int8`, `int16`, `int32`, and `int64`.
  class Int_type : public Type
  {
  public:
    enum Rank
    {
      int8 = 8,
      int16 = 16,
      int32 = 32,
      int64 = 64,
      int128 = 128,
    };

    /// Construct an integer type with the given precision.
    Int_type(Rank r) : Type(int_kind), m_rank(r) { }

    /// Returns the rank of the integer type.
    Rank get_rank() const { return m_rank; }

  private:
    Rank m_rank;
  };


  /// Represents the set of floating point types.
  class Float_type : public Type
  {
  public:
    enum Rank
    {
      float16 = 16,
      float32 = 32,
      float64 = 64,
      float128 = 128,
    };

    /// Construct a float type with the given precision.
    Float_type(Rank r) : Type(float_kind), m_rank(r) { }

    /// Returns the rank of the floating point type.
    Rank get_rank() const { return m_rank; }

  private:
    Rank m_rank;
  };


  /// Represents the singleton type `bool`.
  class Auto_type : public Type
  {
  public:
    Auto_type() : Type(auto_kind) { }
  };


  /// Represents the set of function types.
  class Function_type : public Type
  {
  public:
    Function_type(Type_seq&& parms, Type* ret)
      : Type(func_kind), m_parms(std::move(parms)), m_ret(ret)
    { }

    /// Returns the list of parameter types.
    const Type_seq& get_parameter_types() const { return m_parms; }
    
    /// Returns the list of parameter types.
    Type_seq& get_parameter_types() { return m_parms; }

    /// Returns the return type.
    Type* get_return_type() { return m_ret; }

  private:
    /// The types of parameters.
    Type_seq m_parms;

    /// The return type.
    Type* m_ret;
  };


  // class Reference_type : Type
  // {

  // };

} // namespace beaker
