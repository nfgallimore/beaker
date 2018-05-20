#include "semantics.hpp"
#include "type.hpp"
#include "expression.hpp"
#include "type_specifier.hpp"
#include "context.hpp"
#include "print.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

namespace beaker
{
  Type_specifier*
  Semantics::on_reference_type(Type_specifier* ts, const Token& tok)
  {
    Type* t = m_cxt.get_reference_type(ts->get_type());
    return new Reference_type_specifier(t, ts, tok.get_location());
  }

  Type_specifier*
  Semantics::on_unit_type(const Token& tok)
  {
    return new Simple_type_specifier(m_cxt.get_unit_type(), tok.get_location());
  }
  
  Type_specifier*
  Semantics::on_bool_type(const Token& tok)
  {
    return new Simple_type_specifier(m_cxt.get_bool_type(), tok.get_location());
  }
  
  Type_specifier*
  Semantics::on_int_type(const Token& tok)
  {
    return new Simple_type_specifier(m_cxt.get_int_type(), tok.get_location());
  }
  
  Type_specifier*
  Semantics::on_float_type(const Token& tok)
  {
    return new Simple_type_specifier(m_cxt.get_float_type(), tok.get_location());
  }
  
  Type_specifier*
  Semantics::on_char_type(const Token& tok)
  {
    __builtin_unreachable();
  }

  Type_specifier*
  Semantics::on_function_type(Type_specifier_seq&& parms,
                              Type_specifier* ret,
                              const Token& lparen,
                              const Token& rparen,
                              const Token& arrow)
  {
    /// Build the function type.
    Type_seq ptypes(parms.size());
    std::transform(parms.begin(), parms.end(), ptypes.begin(), [](Type_specifier* ts) {
      return ts->get_type();
    });
    Type* rtype = ret->get_type();
    Type* t = m_cxt.get_function_type(std::move(ptypes), rtype);

    /// Construct the type specifier.
    return new Function_type_specifier(t, std::move(parms), ret, 
                                       lparen.get_location(), 
                                       rparen.get_location(), 
                                       arrow.get_location());
  }

  Type_specifier*
  Semantics::on_paren_type(Type_specifier* ts, 
                           const Token& lparen,
                           const Token& rparen)
  {
    // FIXME: Actually represent the parens explicitly?
    return ts;
  }

  // Type requirements

  Expression*
  Semantics::require_reference(Expression* e)
  {
    Type* t = e->get_type();
    if (t->is_reference())
      return e;
    
    std::stringstream ss;
    ss << *t << " is not a reference type";
    throw std::runtime_error(ss.str());
  }

  Expression*
  Semantics::require_reference_to(Expression* e, Type* t)
  {
    // Make sure that e is a reference.
    e = require_reference(e);
    
    Reference_type* r = static_cast<Reference_type*>(t);
    if (r->is_reference_to(t))
      return e;
    
    std::stringstream ss;
    ss << *t << " is not a reference to " << *t;
    throw std::runtime_error(ss.str());
  }

  Expression*
  Semantics::require_integer(Expression* e)
  {
    /// Convert references to values.
    e = convert_to_value(e);

    Type* t = e->get_type();
    if (t->is_integer())
      return e;
    
    std::stringstream ss;
    ss << *t << " is not an integer type";
    throw std::runtime_error(ss.str());
  }

  Expression_pair
  Semantics::require_common_integer(Expression* e1, Expression* e2)
  {
    std::tie(e1, e2) = convert_to_common_type(e1, e2);
    assert(e1->get_type() == e2->get_type());
    
    Type* t = e1->get_type();
    if (t->is_integer())
      return {e1, e2};

    std::stringstream ss;
    ss << "common type " << *t << " is not an integer type";
    throw std::runtime_error(ss.str());
  }

  Expression_pair
  Semantics::require_same_value(Expression* e1, Expression* e2)
  {
    e1 = convert_to_value(e1);
    e2 = convert_to_value(e2);
    if (e1->get_type() == e2->get_type())
      return {e1, e2};
    
    std::stringstream ss;
    ss << "" << *e1->get_type() << " is not the same as " << *e2->get_type();
    throw std::runtime_error(ss.str());
  }

} // namespace beaker
