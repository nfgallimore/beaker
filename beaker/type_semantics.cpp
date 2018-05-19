#include "semantics.hpp"
#include "expression.hpp"
#include "type.hpp"
#include "type_specifier.hpp"
#include "context.hpp"
#include "print.hpp"

#include <iostream>
#include <sstream>

namespace beaker
{
  Type_specifier*
  Semantics::on_unit_type(const Token& tok)
  {
    return new Simple_type_specifier(tok, m_cxt.get_unit_type());
  }
  
  Type_specifier*
  Semantics::on_bool_type(const Token& tok)
  {
    return new Simple_type_specifier(tok, m_cxt.get_bool_type());
  }
  
  Type_specifier*
  Semantics::on_int_type(const Token& tok)
  {
    return new Simple_type_specifier(tok, m_cxt.get_int_type());
  }
  
  Type_specifier*
  Semantics::on_float_type(const Token& tok)
  {
    return new Simple_type_specifier(tok, m_cxt.get_float_type());
  }
  
  Type_specifier*
  Semantics::on_char_type(const Token& tok)
  {
    __builtin_unreachable();
  }

} // namespace beaker
