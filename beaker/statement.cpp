#include "statement.hpp"

namespace beaker
{

  const char*
  Statement::get_kind_name() const
  {
    switch (m_kind) {
    case block_kind: return "block-statement";
    default:
      break;
    }
    __builtin_unreachable();
  }

} // namespace beaker
