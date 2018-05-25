#include "semantics.hpp"
#include "declaration.hpp"
#include "type.hpp"
#include "expression.hpp"
#include "initializer.hpp"
#include "context.hpp"
#include "print.hpp"

#include <iostream>
#include <sstream>

namespace beaker
{
  void
  Semantics::default_initialize(Data_declaration* d)
  {
    if (d->is_value()) {
      std::stringstream ss;
      ss << "value declarations must be initialized";
      throw std::runtime_error(ss.str());
    }

    if (d->is_reference()) {
      std::stringstream ss;
      ss << "reference declarations must be initialized";
      throw std::runtime_error(ss.str());
    }

    // Build the object expression.
    Expression* obj = make_id_expression(d);

    // Build the initializer. 
    Initializer* init = new Default_initializer(obj);
    d->set_initializer(init);
  }

  /// Return the type of declared entity. For references, the type is
  /// adjusted to be a reference to the declared type.
  static Type*
  get_entity_type(Context& cxt, Data_declaration* d)
  {
    Type* t = d->get_type();
    if (d->is_reference())
      return cxt.get_reference_type(t);
    return t;
  }
  
  /// The value expression is converted to the declared type.
  void
  Semantics::value_initialize(Data_declaration* d, Expression* e)
  {
    // Convert the initializer to the entity's type.
    e = convert_to_type(e, get_entity_type(m_cxt, d));

    // Build the object expression.
    Expression* obj = make_id_expression(d);

    // Build the initializer.
    Initializer* init = new Value_initializer(obj, e);
    d->set_initializer(init);
  }

} // namespace beaker
