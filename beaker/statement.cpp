#include "statement.hpp"

namespace beaker
{

  const char*
  Statement::get_kind_name() const
  {
    switch (m_kind) {
    case block_kind: return "block-statement";
    case ret_kind: return "return-statement";
    case expr_kind: return "expression-statement";
    default:
      break;
    }
    __builtin_unreachable();
  }

} // namespace beaker
