#include "conversion.hpp"

namespace beaker
{

  const char*
  Conversion::get_conversion_name() const
  {
    switch (get_kind()) {
    case value_conv: return "value-conversion";
    case bool_conv: return "bool-conversion";
    case int_prom: return "int-promotion";
    case sign_ext: return "sign-extension";
    case zero_ext: return "zero-extension";
    case int_trunc: return "int-truncation";
    case float_prom: return "float-promotion";
    case float_dem: return "float-demotion";
    case float_ext: return "float-extension";
    case float_trunc: return "float-truncation";
    default:
      break;
    }
    __builtin_unreachable();
  }

} // namespace beaker
