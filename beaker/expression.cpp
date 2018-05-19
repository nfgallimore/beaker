#include "expression.hpp"
#include "conversion.hpp"
#include "dump.hpp"

#include <iostream>

namespace beaker
{
  const char*
  Expression::get_kind_name() const
  {
    switch (m_kind) {
    case bool_kind: return "bool-literal";
    case int_kind: return "int-literal";
    case id_kind: return "id-expression";
    case cond_kind: return "conditional-expression";
    case and_kind: return "logical-and-expression";
    case or_kind: return "logical-or-expression";
    case not_kind: return "logical-not-expression";
    case conv_kind: return "conversion";
    default: __builtin_unreachable();
    }
  }

  static bool
  is_narrowing(const Conversion* conv)
  {
    switch (conv->get_operation()) {
    case Conversion::value_conv:
    case Conversion::int_prom:
    case Conversion::sign_ext:
    case Conversion::zero_ext:
    case Conversion::float_prom:
    case Conversion::float_ext:
      return false;
    case Conversion::bool_conv:
    case Conversion::int_trunc:
    case Conversion::float_dem:
    case Conversion::float_trunc:
      return true;
    }
  }

  /// Note that a value conversion is not a narrowing conversion.
  bool
  Expression::is_narrowing_conversion() const
  {
    if (m_kind == conv_kind)
      return is_narrowing(static_cast<const Conversion*>(this));
    return false;
  }

  static bool
  is_widening(const Conversion* conv)
  {
    switch (conv->get_operation()) {
    case Conversion::int_prom:
    case Conversion::sign_ext:
    case Conversion::zero_ext:
    case Conversion::float_prom:
    case Conversion::float_ext:
      return true;
    case Conversion::value_conv:
    case Conversion::bool_conv:
    case Conversion::int_trunc:
    case Conversion::float_dem:
    case Conversion::float_trunc:
      return false;
    }
  }

  /// Note that a value conversion is not a widening conversion.
  bool
  Expression::is_widening_conversion() const
  {
    if (m_kind == conv_kind)
      return is_widening(static_cast<const Conversion*>(this));
    return false;
  }

  void
  Expression::dump() const
  {
    Dump_context dc(std::cerr);
    beaker::dump(dc, this);
  }

} // namespace beaker
