#include "semantics.hpp"
#include "conversion.hpp"
#include "type.hpp"
#include "context.hpp"
#include "print.hpp"

#include <iostream>
#include <sstream>

namespace beaker
{
  Expression*
  Semantics::convert_to_value(Expression* e)
  {
    if (Reference_type* rt = dynamic_cast<Reference_type*>(e->get_type()))
      return new Value_conversion(rt->get_object_type(), e);
    return e;
  }

  Expression*
  Semantics::convert_to_bool(Expression* e)
  {
    e = convert_to_value(e);

    Type* t = e->get_type();
    switch (t->get_kind()) {
    case Type::bool_kind:
      return e;
    
    case Type::int_kind:
    case Type::float_kind:
    case Type::func_kind:
      return new Bool_conversion(m_cxt.get_bool_type(), e);

    default:
      break;
    }

    std::stringstream ss;
    ss << "cannot convert " << '\'' << *t << '\'' << " to bool";
    throw std::runtime_error(ss.str());
  }

} // namespace beaker
