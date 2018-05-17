#include "type.hpp"

namespace beaker
{
  const char* 
  Type::get_kind_name() const
  {
    switch (m_kind) {
    case unit_kind: return "unit-type";
    case bool_kind: return "bool-type";
    case int_kind: return "int-type";
    case float_kind: return "float-type";
    case auto_kind: return "auto-type";
    case func_kind: return "function-type";
    case ref_kind: return "reference-type";
    }
  }

} // namespace beaker
