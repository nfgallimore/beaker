#include "statement.hpp"
#include "dump.hpp"

#include <iostream>

namespace beaker
{

  const char*
  Statement::get_kind_name() const
  {
    switch (m_kind) {
    case block_kind: return "block-statement";
    case when_kind: return "when-statement";
    case if_kind: return "if-statement";
    case while_kind: return "while-statement";
    case break_kind: return "break-statement";
    case cont_kind: return "continue-statement";
    case ret_kind: return "return-statement";
    case expr_kind: return "expression-statement";
    case decl_kind: return "declaration-statement";
    default:
      break;
    }
    assert(false);
  }

  void
  Statement::dump() const
  {
    Dump_context dc(std::cerr);
    beaker::dump(dc, this);
  }

} // namespace beaker
