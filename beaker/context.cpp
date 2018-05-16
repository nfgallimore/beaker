#include "context.hpp"
#include "type.hpp"

namespace beaker
{
  Context::Context()
    : m_syms(),
      m_unit_type(new Unit_type()),
      m_bool_type(new Bool_type()),
      m_int_type(new Int_type(Int_type::int32)),
      m_float_type(new Float_type(Float_type::float32)),
      m_auto_type(new Auto_type())
  { }

  Context::~Context()
  { }

} // namespace beaker
