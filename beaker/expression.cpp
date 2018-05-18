#include "expression.hpp"
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
    case and_kind: return "logical-and-expression";
    case or_kind: return "logical-or-expression";
    case not_kind: return "logical-not-expression";
    case conv_kind: return "conversion";
    default: __builtin_unreachable();
    }
  }

  void
  Expression::dump() const
  {
    Dump_context dc(std::cerr);
    beaker::dump(dc, this);
  }

} // namespace beaker
