#include "evaluation.hpp"
#include "declaration.hpp"

namespace beaker
{
  void
  Static_store::bind(const Typed_declaration* d, const Value& v)
  {
    assert(!d->is_variable());
    assert(m_values.count(d) == 0);
    do_bind(d, v);
  }

  Object*
  Static_store::create(const Variable_declaration* d)
  {
    assert(m_values.count(d) == 0);

    // Create the uninitialized object.
    m_objects.emplace_front(Creator(d));
    Object* obj = &m_objects.front();

    // Bind the name to its object (i.e., address).
    do_bind(d, Value(obj));

    return obj;
  }

  Value
  Static_store::get(const Typed_declaration* d)
  {
    assert(m_values.count(d) == 1);
    return m_values.find(d)->second;
  }

  boost::optional<Value>
  Static_store::get_if(const Typed_declaration* d)
  {
    auto iter = m_values.find(d);
    if (iter != m_values.end())
      return iter->second;
    return {};
  }

  void
  Static_store::do_bind(const Typed_declaration* d, const Value& v)
  {
    m_values.emplace(d, v);
  }

  Value
  Evaluator::fetch(const Typed_declaration* d)
  {
    if (const Data_declaration* dd = dynamic_cast<const Data_declaration*>(d))
      return fetch_data(dd);
    else if (const Function_declaration* fn = dynamic_cast<const Function_declaration*>(d))
      return fetch_function(fn);
    assert(false);
  }

  Value
  Evaluator::fetch_data(const Data_declaration* d)
  {
    if (d->has_static_storage())
      return fetch_static(d);
    else
      return fetch_automatic(d);
  }

  /// Note that this is used for both static data and functions.
  Value
  Evaluator::fetch_static(const Typed_declaration* d)
  {
    // If we've previously bound the declaration, return the value.
    if (boost::optional<Value> val = m_statics.get_if(d))
      return *val;

    // Otherwise, we need to lazily elaborate the declaration.
    elaborate(d);

    // Return the newly bound value.
    return m_statics.get(d);
  }

  Value
  Evaluator::fetch_automatic(const Data_declaration* d)
  {
    // FIXME: Implement me.
    assert(false);
  }

  Value
  Evaluator::fetch_function(const Function_declaration* fn)
  {
    return fetch_static(fn);
  }

} // namespace beaker
