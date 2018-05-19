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
      ss << "value declaration must be initialized";
      throw std::runtime_error(ss.str());
    }

    // FIXME: Actually analyze the type of D to see if it can be default
    // initialized (what can't?).
    Initializer* init = new Default_initializer();
    d->set_initializer(init);
  }

  /// The value expression is converted to the declared type.
  void
  Semantics::value_initialize(Data_declaration* d, Expression* e)
  {
    // FIXME: What else?
    e = convert_to_type(e, d->get_type());

    Initializer* init = new Value_initializer(e);
    d->set_initializer(init);
  }

} // namespace beaker
