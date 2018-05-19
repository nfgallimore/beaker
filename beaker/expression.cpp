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

    // id expressions
    case id_kind: return "id-expression";

    // arithmetic expressions
    case add_kind: return "addition-expression";
    case sub_kind: return "subtraction-expression";
    case mul_kind: return "multiplication-expression";
    case quo_kind: return "quotient-expression";
    case rem_kind: return "remainder-expression";
    case neg_kind: return "negation-expression";
    case rec_kind: return "reciprocal-expression";

    // bitwise expressions
    case bit_and_kind: return "bitwise-and-expression";
    case bit_ior_kind: return "bitwise-or-expression";
    case bit_xor_kind: return "bitwise-xor-expression";
    case bit_not_kind: return "bitwise-not-expression";
    case bit_shl_kind: return "shift-left-expression";
    case bit_shr_kind: return "shift-right-expression";

    case cond_kind: return "conditional-expression";
    case and_kind: return "logical-and-expression";
    case or_kind: return "logical-or-expression";
    case not_kind: return "logical-not-expression";

      // relational expressions
    case eq_kind: return "equal-to-expression";
    case ne_kind: return "not-equal-to-expression";
    case lt_kind: return "less-than-expression";
    case gt_kind: return "greater-than-expression";
    case ng_kind: return "not-greather-than-expression";
    case nl_kind: return "not-less-than-expression";

      // object expressions
    case assign_kind: return "assignment-expression";

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

    case def_init: return "default-initializer";
    case val_init: return "value-initializer";
    
    default: __builtin_unreachable();
    }
  }

  /// Note that a value conversion is not a narrowing conversion.
  bool
  Expression::is_narrowing_conversion() const
  {
    switch (m_kind) {
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
  Expression::is_widening_conversion() const
  {
    switch (m_kind) {
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

  void
  Expression::dump() const
  {
    Dump_context dc(std::cerr);
    beaker::dump(dc, this);
  }

} // namespace beaker
