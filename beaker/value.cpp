#include "value.hpp"

#include <iostream>

namespace beaker
{

  std::ostream&
  operator<<(std::ostream& os, const Value& v)
  {
    switch (v.get_kind()) {
    case Value::indet_kind:
      return os << "<indeterminate>";
    case Value::int_kind:
      return os << v.get_int();
    case Value::float_kind:
      return os << v.get_float();
    case Value::func_kind:
      return os << "fn@" << v.get_function();
    case Value::ref_kind:
      return os << "obj@" << v.get_reference();
    }
  }

} // namespace beaker
