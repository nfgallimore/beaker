#include "evaluation.hpp"
#include "declaration.hpp"
#include "initializer.hpp"
#include "value.hpp"
#include "object.hpp"

namespace beaker
{
  void
  Evaluator::elaborate(const Declaration* d)
  {
    switch (d->get_kind()) {
    default:
      break;
    case Declaration::func_kind:
      return elaborate_function(static_cast<const Function_declaration*>(d));
    
    case Declaration::val_kind:
    case Declaration::ref_kind:
      return elaborate_constant(static_cast<const Data_declaration*>(d));
    
    case Declaration::var_kind:
      return elaborate_variable(static_cast<const Variable_declaration*>(d));
    
    case Declaration::parm_kind:
      // FIXME: These are elaborated during function call, so we shouldn't
      // get here in the general case.
      break;
    }
    assert(false);
  }

  void
  Evaluator::elaborate_function(const Function_declaration* d)
  {
    Value init(d);
    m_statics.bind(d, init);
  }

  void
  Evaluator::elaborate_variable(const Variable_declaration* d)
  {
    // Allocate the object and initialize it by applying the initializer.
    m_statics.create(d);
    evaluate(d->get_initializer());
  }

  void
  Evaluator::elaborate_constant(const Data_declaration* d)
  {
    // Evaluate the initializer and directly bind the declaration to that value.
    Value init = evaluate(d->get_initializer());
    m_statics.bind(d, init);
  }

} // namespace beaker
