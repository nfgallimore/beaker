#include "semantics.hpp"
#include "conversion.hpp"
#include "type.hpp"
#include "context.hpp"
#include "print.hpp"

#include <iostream>
#include <sstream>

namespace beaker
{
  /// Returns the integer type of the expression.
  static Int_type*
  get_int_type(Expression* e)
  {
    Type* t = e->get_type();
    assert(t->is_integer());
    return static_cast<Int_type*>(t);
  }

  /// Returns the integer rank of the expression.
  static Int_type::Rank
  get_int_rank(Expression* e)
  {
    return get_int_type(e)->get_rank();
  }

  /// Returns the floating point type of the expression.
  static Float_type*
  get_fp_type(Expression* e)
  {
    Type* t = e->get_type();
    assert(t->is_floating_point());
    return static_cast<Float_type*>(t);
  }

  /// Returns the floating point rank of the expression.
  static Float_type::Rank
  get_fp_rank(Expression* e)
  {
    return get_fp_type(e)->get_rank();
  }

  /// Returns the reference type of the expression.
  static Reference_type*
  get_ref_type(Expression* e)
  {
    Type* t = e->get_type();
    assert(t->is_reference());
    return static_cast<Reference_type*>(t);
  }

  /// Returns the reference type of the expression.
  static Reference_type*
  get_if_ref_type(Expression* e)
  {
    Type* t = e->get_type();
    if (t->is_reference())
      return static_cast<Reference_type*>(t);
    return nullptr;
  }

  Expression*
  Semantics::convert_to_type(Expression* e, Type* t)
  {
    switch (t->get_kind()) {
    case Type::unit_kind:
    case Type::auto_kind:
    case Type::func_kind:
      break;
    case Type::bool_kind:
      return convert_to_bool(e);
    case Type::int_kind:
      return convert_to_integer(e, static_cast<Int_type*>(t));
    case Type::float_kind:
      return convert_to_floating_point(e, static_cast<Float_type*>(t));
    case Type::ref_kind:
      return convert_to_reference(e, static_cast<Reference_type*>(t));
    }

    std::stringstream ss;
    ss << "cannot convert " << *e->get_type() << " to " << *t;
    throw std::runtime_error(ss.str());
  }

  Expression*
  Semantics::convert_to_value(Expression* e)
  {
    if (Reference_type* rt = get_if_ref_type(e))
      return new Value_conversion(rt->get_object_type(), e);
    return e;
  }

  Expression*
  Semantics::convert_to_bool(Expression* e)
  {
    // Convert references to values.
    e = convert_to_value(e);

    Type* t = e->get_type();
    Type* b = m_cxt.get_bool_type();

    switch (t->get_kind()) {
    case Type::bool_kind:
      // No conversion necessary.
      return e;
    
    case Type::int_kind:
    case Type::float_kind:
    case Type::func_kind:
      return new Bool_conversion(b, e);

    default:
      break;
    }

    std::stringstream ss;
    ss << "cannot convert " << *t << " to bool";
    throw std::runtime_error(ss.str());
  }

  Expression*
  Semantics::convert_to_integer(Expression* e, Int_type* z)
  {
    // Convert references to values.
    e = convert_to_value(e);

    Type* t = e->get_type();
    switch (t->get_kind()) {
    case Type::bool_kind:
      return new Int_promotion(z, e);

    case Type::float_kind:
      return new Float_demotion(z, e);
    
    case Type::int_kind:
      return convert_integer(e, z);
    
    default:
      break;
    }

    std::stringstream ss;
    ss << "cannot convert " << *t << " to " << *z;
    throw std::runtime_error(ss.str());
  }

  Expression*
  Semantics::convert_integer(Expression* e, Int_type* z)
  {
    Int_type* t = get_int_type(e);
    if (t->get_rank() < z->get_rank())
      return extend_integer(e, z);
    if (t->get_rank() > z->get_rank())
      return truncate_integer(e, z);
    return e;
  }

  Expression*
  Semantics::extend_integer(Expression* e, Int_type* z)
  {
    assert(get_int_rank(e) < z->get_rank());
    return new Sign_extension(z, e);
  }

  Expression*
  Semantics::truncate_integer(Expression* e, Int_type* z)
  {
    assert(get_int_rank(e) > z->get_rank());
    return new Int_truncation(z, e);
  }

  Expression*
  Semantics::convert_to_floating_point(Expression* e, Float_type* f)
  {
    // Convert references to values.
    e = convert_to_value(e);

    Type* t = e->get_type();
    switch (t->get_kind()) {
    case Type::float_kind:
      return convert_floating_point(e, f);
    
    case Type::int_kind:
      return new Float_promotion(f, e);
    
    default:
      break;
    }

    std::stringstream ss;
    ss << "cannot convert " << *t << " to " << *f;
    throw std::runtime_error(ss.str());
  }

  Expression*
  Semantics::convert_floating_point(Expression* e, Float_type* f)
  {
    Float_type* t = get_fp_type(e);
    if (t->get_rank() < f->get_rank())
      return extend_floating_point(e, f);
    if (t->get_rank() > f->get_rank())
      return truncate_floating_point(e, f);
    return e;
  }

  Expression*
  Semantics::extend_floating_point(Expression* e, Float_type* f)
  {
    assert(get_fp_rank(e) < f->get_rank());
    return new Float_extension(f, e);
  }

  Expression*
  Semantics::truncate_floating_point(Expression* e, Float_type* f)
  {
    assert(get_fp_rank(e) > f->get_rank());
    return new Float_truncation(f, e);
  }

  /// Returns true if t1 and t2 are both references.
  static bool
  both_references(const Type* t1, const Type* t2)
  {
    return t1->is_reference() && t2->is_reference();
  }

  Expression*
  Semantics::convert_to_reference(Expression* e, Reference_type* r)
  {
    if (Reference_type* t = get_if_ref_type(e)) {
      // References to the same object do not require conversions.
      if (t->get_object_type() == r->get_object_type())
        return e;

      // FIXME: If we have reference subtyping, allow a reference to
      // bind to a supertype.
    }

    // A non-reference can never converted to a reference.

    std::stringstream ss;
    ss << "cannot convert " << *e->get_type() << " to " << *r;
    throw std::runtime_error(ss.str());
  }

  Expression_pair
  Semantics::convert_to_common_type(Expression* e1, Expression* e2)
  {
    Type* t1 = e1->get_type();
    Type* t2 = e2->get_type();

    // If e1 and e2 have the same type, then no conversions are needed.
    if (t1 == t2)
      return {e1, e2};

    // Convert to a common reference type.
    if (both_references(t1, t2))
      return convert_to_common_reference(e1, e2);

    // Convert to a common value type.
    return convert_to_common_value(e1, e2);
  }

  Expression_pair
  Semantics::convert_to_common_reference(Expression* e1, Expression* e2)
  {
    Reference_type* t1 = get_ref_type(e1);
    Reference_type* t2 = get_ref_type(e2);
    
    // If t1 and t2 refer to objects of the same type then no conversions
    // are needed.
    if (t1->get_object_type() == t2->get_object_type())
      return {e1, e2};

    // FIXME: If we have subtyping rules for references, the common type 
    // is the least upper bound.

    std::stringstream ss;
    ss << "no common reference type for " << *t1 << " and " << *t2;
    throw std::runtime_error(ss.str());
  }

  /// Returns true if both types are integers point types.
  static bool
  both_integers(const Type* t1, const Type* t2)
  {
    return t1->is_integer() && t2->is_integer();
  }

  /// Returns true if both types are floating point types.
  static bool
  both_floating_point(const Type* t1, const Type* t2)
  {
    return t1->is_floating_point() && t2->is_floating_point();
  }

  Expression_pair
  Semantics::convert_to_common_value(Expression* e1, Expression* e2)
  {
    // Convert both expressions to values.
    e1 = convert_to_value(e1);
    e2 = convert_to_value(e2);

    Type* t1 = e1->get_type();
    Type* t2 = e2->get_type();

    // If t1 and t2 are the same value types, then no conversions are
    // needed.
    if (t1 == t2)
      return {e1, e2};

    // If both expressions are integer expressions, the expression
    // with the lower rank is converted to the type of the greater.
    if (both_integers(t1, t2))
      return convert_to_common_integer(e1, e2);

    // If both expressions are floating point expressions, the expression
    // with the lower rank is converted to the type of the greater.
    if (both_floating_point(t1, t2))
      return convert_to_common_floating_point(e1, e2);

    // If one expression is a boolean expression and the is an integer
    // expression, the integer expression is converted to bool.
    if (t1->is_bool() && t2->is_integer())
      return {e1, convert_to_bool(e2)};
    if (t1->is_integer() && t2->is_bool())
      return {convert_to_bool(e1), e2};
    
    // If one expression is floating point expression and the other is 
    // an integer expression, the integer expression is converted to the
    // type of the other.
    if (t1->is_floating_point() && t2->is_integer())
      return {e1, convert_to_floating_point(e2, get_fp_type(e1))};
    if (t1->is_integer() && t2->is_floating_point())
      return {convert_to_floating_point(e1, get_fp_type(e2)), e2};

    std::stringstream ss;
    ss << "no common type for " << *t1 << " and " << *t2;
    throw std::runtime_error(ss.str());
  }

  Expression_pair
  Semantics::convert_to_common_integer(Expression* e1, Expression* e2)
  {
    Int_type* t1 = get_int_type(e1);
    Int_type* t2 = get_int_type(e2);
    if (t1->get_rank() < t2->get_rank())
      return {extend_integer(e1, t2), e2};
    if (t1->get_rank() > t2->get_rank())
      return {e1, extend_integer(e2, t1)};
    return {e1, e2};
  }

  Expression_pair
  Semantics::convert_to_common_floating_point(Expression* e1, Expression* e2)
  {
    Float_type* t1 = get_fp_type(e1);
    Float_type* t2 = get_fp_type(e2);
    if (t1->get_rank() < t2->get_rank())
      return {extend_floating_point(e1, t2), e2};
    if (t1->get_rank() > t2->get_rank())
      return {e1, extend_floating_point(e2, t1)};
    return {e1, e2};
  }

} // namespace beaker
