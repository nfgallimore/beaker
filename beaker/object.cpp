#include "object.hpp"
#include "expression.hpp"
#include "declaration.hpp"

namespace beaker
{

  const Type*
  Object::get_type() const
  {
    if (m_creator.is_declaration()) {
      const Data_declaration* d = m_creator.get_declaration();
      return d->get_type();
    }
    else if (m_creator.is_expression()) {
      const Expression* e = m_creator.get_expression();
      return e->get_type();
    }
    else {
      // The object may have been created by the system.
      assert(false);
    }
  }

} // namespace beaker
