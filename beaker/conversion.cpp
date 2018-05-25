#include "conversion.hpp"

namespace beaker
{

  const char*
  Conversion::get_conversion_name() const
  {
    switch (m_conv) {
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
    default: break;
    }
    __builtin_unreachable();
  }


  /// Note that a value conversion is not a narrowing conversion.
  bool
  Conversion::is_narrowing_conversion() const
  {
    switch (m_conv) {
    default:
      return false;
    case bool_conv:
    case int_trunc:
    case float_dem:
    case float_trunc:
      return true;
    }  
  }

  /// Note that a value conversion is not a widening conversion.
  bool
  Conversion::is_widening_conversion() const
  {
    switch (m_conv) {
    default:
      return false;
    case int_prom:
    case sign_ext:
    case zero_ext:
    case float_prom:
    case float_ext:
      return true;
    }  
  }


} // namespace beaker
