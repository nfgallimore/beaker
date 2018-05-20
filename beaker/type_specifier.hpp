#pragma once

#include <beaker/common.hpp>
#include <beaker/location.hpp>

namespace beaker
{
  /// The base class of all type specifiers.
  class Type_specifier
  {
  public:
    enum Kind
    {
      simple_kind,
      ref_kind,
      func_kind,
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
    Simple_type_specifier(Type* t, Location loc)
      : Type_specifier(simple_kind, t)
    { }

    /// Returns the start location of the this statement.
    Location get_start_location() const override { return m_loc; }
    
    /// Returns the end location of the this statement.
    Location get_end_location() const override { return m_loc; }

  private:
    Location m_loc;
  };


  /// Represents reference specifiers of the form `ref ts`.
  class Reference_type_specifier : public Type_specifier
  {
  public:
    Reference_type_specifier(Type* t, Type_specifier* ts, Location loc)
      : Type_specifier(ref_kind, t), m_ts(ts), m_loc(loc)
    { }

    /// Returns the underlying specifier for the value type.
    Type_specifier* get_value_type() const { return m_ts; }

    /// Returns the start location of the this statement.
    Location get_start_location() const override { return m_loc; }
    
    /// Returns the end location of the this statement.
    Location get_end_location() const override { return m_ts->get_end_location(); }

  private:
    /// The underlying type specifier.
    Type_specifier* m_ts;
    
    /// The location of the `ref` token.
    Location m_loc;
  };


  /// Represents type specifiers of the form `(t1, t2, ..., tn) -> tr`.
  class Function_type_specifier : public Type_specifier
  {
  public:
    Function_type_specifier(Type* t,
                            const Type_specifier_seq& parms,
                            Type_specifier* ret, 
                            Location lparen, 
                            Location rparen, 
                            Location arrow);

    Function_type_specifier(Type* t,
                            Type_specifier_seq&& parms,
                            Type_specifier* ret, 
                            Location lparen, 
                            Location rparen, 
                            Location arrow);

    /// Returns the parameter types of the specifier.
    const Type_specifier_seq& get_parameter_types() const { return m_parms; }

    /// Returns the parameter types of the specifier.
    Type_specifier_seq& get_parameter_types() { return m_parms; }

    /// Returns the return type specifier.
    Type_specifier* get_return_type() const { return m_ret; }

    /// Returns the start location of the this statement. This is the location
    /// of the left paren.
    Location get_start_location() const override { return m_locs[0]; }
    
    /// Returns the end location of the this statement. This is the end 
    /// location of the return type specifier.
    Location get_end_location() const override { return m_ret->get_end_location(); }

  private:
    /// The parameter type specifiers.
    Type_specifier_seq m_parms;
    
    /// The return type specifier.
    Type_specifier* m_ret;
    
    /// The locations of the left paren, right paren, and arrow tokens,
    /// respectively.
    Location m_locs[3];
  };

  inline
  Function_type_specifier::
  Function_type_specifier(Type* t,
                          const Type_specifier_seq& parms,
                          Type_specifier* ret, 
                          Location lparen, 
                          Location rparen, 
                          Location arrow)
    : Type_specifier(func_kind, t), 
      m_parms(parms), 
      m_ret(ret),
      m_locs{lparen, rparen, arrow}
  { }

  inline
  Function_type_specifier::
  Function_type_specifier(Type* t,
                          Type_specifier_seq&& parms,
                          Type_specifier* ret, 
                          Location lparen, 
                          Location rparen, 
                          Location arrow)
    : Type_specifier(func_kind, t), 
      m_parms(std::move(parms)), 
      m_ret(ret),
      m_locs{lparen, rparen, arrow}
  { }

} // namespace beaker
