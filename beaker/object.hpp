#pragma once

#include <beaker/type.hpp>
#include <beaker/value.hpp>

namespace beaker
{
  /// An object occupies a region of storage and holds a value. Every object
  /// has a type, which describes the amount of storage needed and how the
  /// object can be accessed.
  ///
  /// \todo Implement aggregate objects; An object is either an aggregate
  /// (comprised of other objects), or it is a scalar (having a value).
  class Object
  {
  public:
    Object(Type* t)
      : m_type(t), m_value()
    { }

    Object(Type* t, const Value& v)
      : m_type(t), m_value(v)
    { }

    Object(Type* t, Value&& v)
      : m_type(t), m_value(std::move(v))
    { }

    /// Returns the type of the object.
    Type* get_type() const { return m_type; }

    /// Provides an initial value for the object.
    void initialize(const Value& v);

    /// Update the value of the object.
    void store(const Value& v) { m_value = v; }

    /// Returns the value of the object.
    const Value& load() const { return m_value; }

  private:
    /// The type of object.
    Type* m_type;

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
