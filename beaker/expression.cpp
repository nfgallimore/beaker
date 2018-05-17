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
    }
  }

  void
  Expression::dump() const
  {
    Dump_context dc(std::cerr);
    beaker::dump(dc, this);
  }

} // namespace beaker
