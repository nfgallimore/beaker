#pragma once

#include <beaker/type.hpp>
#include <beaker/value.hpp>

#include <boost/variant.hpp>

namespace beaker
{
  /// An object is created by a declaration or certain expressions.
  class Creator : private boost::variant<const Data_declaration*, const Expression*>
  {
    using Base = boost::variant<const Data_declaration*, const Expression*>;
  public:

    Creator(const Data_declaration* d)
      : Base(d)
    { }

    Creator(const Expression* e)
      : Base(e)
    { }

    /// Returns true if the creator is a declaration.
    bool is_declaration() const { return which() == 0; }

    /// Returns true if the creator is an expression.
    bool is_expression() const { return which() == 1; }

    /// Returns this as a declaration.
    const Data_declaration* get_declaration() const;
    
    /// Returns this as an expression.
    const Expression* get_expression() const;
  };

  inline const Data_declaration*
  Creator::get_declaration() const 
  { 
    return boost::get<const Data_declaration*>(*this); 
  }

  inline const Expression*
  Creator::get_expression() const 
  { 
    return boost::get<const Expression*>(*this); 
  }


  /// An object occupies a region of storage and holds a value. Every object
  /// has a type, which describes the amount of storage needed and how the
  /// object can be accessed.
  ///
  /// \todo Implement aggregate objects; An object is either an aggregate
  /// (comprised of other objects), or it is a scalar (having a value).
  class Object
  {
  public:
    Object(Creator c)
      : m_creator(c), m_value()
    { }

    Object(Creator c, const Value& v)
      : m_creator(c), m_value(v)
    { }

    Object(Creator c, Value&& v)
      : m_creator(c), m_value(std::move(v))
    { }

    /// Returns the creator of the object.
    Creator get_creator() const { return m_creator; }

    /// Returns the type of the object.
    const Type* get_type() const;

    /// Provides an initial value for the object.
    void initialize(const Value& v);

    /// Update the value of the object.
    void store(const Value& v) { m_value = v; }

    /// Returns the value of the object.
    const Value& load() const { return m_value; }

  private:
    /// The creator of the object.
    Creator m_creator;

    /// The value held by the object.
    Value m_value;
  };

  inline void
  Object::initialize(const Value& v)
  {
    assert(m_value.is_indeterminate());
    m_value = v;
  }

} // namespace beaker
