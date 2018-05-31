#include "evaluation.hpp"
#include "expression.hpp"
#include "arithmetic_expression.hpp"
#include "bitwise_expression.hpp"
#include "relational_expression.hpp"
#include "logical_expression.hpp"
#include "conversion.hpp"
#include "initializer.hpp"
#include "declaration.hpp"

namespace beaker
{
  Value
  Evaluator::evaluate(const Expression* e)
  {
    switch (e->get_kind()) {
    case Expression::bool_kind:
      return evaluate_bool_literal(static_cast<const Bool_literal*>(e));
    case Expression::int_kind:
      return evaluate_int_literal(static_cast<const Int_literal*>(e));
    case Expression::id_kind:
      return evaluate_id_expression(static_cast<const Id_expression*>(e));
    case Expression::init_kind:
      return evaluate_init_expression(static_cast<const Init_expression*>(e));

    // arithmetic expressions
    case Expression::add_kind:
      break;
    case Expression::sub_kind:
      break;
    case Expression::mul_kind:
      break;
    case Expression::quo_kind:
      break;
    case Expression::rem_kind:
      break;
    case Expression::neg_kind:
      break;
    case Expression::rec_kind:
      break;

    // bitwise expressions
    case Expression::bit_and_kind:
      break;
    case Expression::bit_ior_kind:
      break;
    case Expression::bit_xor_kind:
      break;
    case Expression::bit_not_kind:
      break;
    case Expression::bit_shl_kind:
      break;
    case Expression::bit_shr_kind:
      break;

    // logical expressions
    case Expression::cond_kind:
      return evaluate_conditional_expression(static_cast<const Conditional_expression*>(e));
    case Expression::and_kind:
      return evaluate_logical_and_expression(static_cast<const Logical_and_expression*>(e));
    case Expression::or_kind:
      return evaluate_logical_or_expression(static_cast<const Logical_or_expression*>(e));
    case Expression::not_kind:
      return evaluate_logical_not_expression(static_cast<const Logical_not_expression*>(e));

    // relational expressions
    case Expression::eq_kind:
      break;
    case Expression::ne_kind:
      break;
    case Expression::lt_kind:
      break;
    case Expression::gt_kind:
      break;
    case Expression::ng_kind:
      break;
    case Expression::nl_kind:
      break;

    // object expressions
    case Expression::assign_kind:
      break;

    // conversions
    case Expression::imp_conv:
      return evaluate_implicit_conversion(static_cast<const Implicit_conversion*>(e));

    // initializers
    case Expression::empty_init:
      break;
    case Expression::def_init:
      break;
    case Expression::val_init:
      return evaluate_value_initializer(static_cast<const Value_initializer*>(e));
    }
    e->dump();
    assert(false);
  }

  Value
  Evaluator::evaluate_bool_literal(const Bool_literal* e)
  {
    return Value(e->get_value());
  }

  Value
  Evaluator::evaluate_int_literal(const Int_literal* e)
  {
    return Value(e->get_value());
  }

  Value
  Evaluator::evaluate_id_expression(const Id_expression* e)
  {
    // FIXME: an id-expression that refers to a reference can fail to
    // be a constant expression.
    return fetch(e->get_declaration());
  }

  Value
  Evaluator::evaluate_init_expression(const Id_expression* e)
  {
    // Always allow the evaluation of references to objects being initialized.
    return fetch(e->get_declaration());
  }


  Value
  Evaluator::evaluate_conditional_expression(const Conditional_expression* e)
  {
    Value cond = evaluate(e->get_condition());
    if (cond.get_int())
      return evaluate(e->get_true_value());
    else
      return evaluate(e->get_false_value());
  }

  Value
  Evaluator::evaluate_logical_and_expression(const Logical_and_expression* e)
  {
    Value lhs = evaluate(e->get_lhs());
    if (lhs.get_int())
      return evaluate(e->get_rhs());
    else
      return lhs;
  }

  Value
  Evaluator::evaluate_logical_or_expression(const Logical_or_expression* e)
  {
    Value lhs = evaluate(e->get_lhs());
    if (lhs.get_int())
      return lhs;
    else
      return evaluate(e->get_rhs());
  }

  Value
  Evaluator::evaluate_logical_not_expression(const Logical_not_expression* e)
  {
    Value arg = evaluate(e->get_operand());
    return Value(!arg.get_int());
  }

  Value
  Evaluator::evaluate_implicit_conversion(const Implicit_conversion* e)
  {
    switch (e->get_conversion_kind()) {
    case Conversion::value_conv:
      return convert_to_value(e->get_source());
    case Conversion::bool_conv:
    case Conversion::int_prom:
    case Conversion::sign_ext:
    case Conversion::zero_ext:
    case Conversion::int_trunc:
    case Conversion::float_prom:
    case Conversion::float_dem:
    case Conversion::float_ext:
    case Conversion::float_trunc:
      break;
    }
    assert(false);
  }

  /// Applies a reference-to-value conversion.
  Value
  Evaluator::convert_to_value(const Expression* e)
  {
    Value ref = evaluate(e);

    // A value-conversion is not a constant expression unless...
    //
    // FIXME: Check that the reference is valid!
    Object* obj = ref.get_reference();

    // ... the source expression refers to data that is initialized by a
    // constant expression.
    //
    // FIXME: This isn't quite right. We probably need to qualify this in
    // terms of the "initialized by a constant expression", which would
    // include all local variables created during initialization.
    Creator c = obj->get_creator();
    if (c.is_declaration()) {
      const Data_declaration* d = c.get_declaration();
      
      // Technically, global variables can have a constant initialization,
      // but we don't know if there have been any intermediate writes.
      if (d->is_variable() && d->has_static_storage())
        throw std::runtime_error("read from non-constant object");
    }

    
    Value val = obj->load();
    if (val.is_indeterminate())
      throw std::runtime_error("use of indeterminate value");
    return val;
  }

  /// FIXME: This returns an indeterminate value, which doesn't quite 
  /// correspond to void, but it's close.
  Value
  Evaluator::evaluate_value_initializer(const Value_initializer* e)
  {
    Value ref = evaluate(e->get_object());
    Value val = evaluate(e->get_value());
    Object* obj = ref.get_reference();
    obj->initialize(val);
    return Value();
  }

} // namespace beaker
