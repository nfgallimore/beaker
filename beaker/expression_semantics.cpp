#include "semantics.hpp"
#include "expression.hpp"
#include "arithmetic_expression.hpp"
#include "bitwise_expression.hpp"
#include "logical_expression.hpp"
#include "type.hpp"
#include "declaration.hpp"
#include "context.hpp"
#include "print.hpp"

#include <iostream>
#include <sstream>

namespace beaker
{
  Expression*
  Semantics::on_assignment_expression(Expression* e1,
                                      Expression* e2,
                                      const Token& op)
  {
    return nullptr;
  }

  Expression*
  Semantics::on_conditional_expression(Expression* e1, 
                                       Expression* e2, 
                                       Expression* e3,
                                       const Token& question,
                                       const Token& colon)
  {
    e1 = convert_to_bool(e1);
    std::tie(e2, e3) = convert_to_common(e2, e3);
    Type* t = e2->get_type();
    return new Conditional_expression(t, e1, e2, e3, question, colon);
  }

  /// The operands are converted to bool values.
  /// The type of the expression is bool.
  Expression*
  Semantics::on_logical_or_expression(Expression* e1, 
                                       Expression* e2,
                                       const Token& op)
  {
    e1 = convert_to_bool(e1);
    e2 = convert_to_bool(e2);
    Type* t = m_cxt.get_bool_type();    
    return new Logical_or_expression(t, e1, e2, op);
  }


  /// The operands are converted to bool values.
  /// The type of the expression is bool.
  Expression*
  Semantics::on_logical_and_expression(Expression* e1, 
                                       Expression* e2,
                                       const Token& op)
  {
    e1 = convert_to_bool(e1);
    e2 = convert_to_bool(e2);
    Type* t = m_cxt.get_bool_type();
    return new Logical_and_expression(t, e1, e2, op);
  }

  Expression*
  Semantics::on_logical_not_expression(Expression* e,
                                       const Token& op)
  {
    e = convert_to_bool(e);
    Type* t = m_cxt.get_bool_type();
    return new Logical_not_expression(t, e, op);
  }

  Expression*
  Semantics::on_bitwise_expression(Expression* e1, 
                                    Expression* e2,
                                    const Token& op)
  {
    return nullptr;
  }

  Expression*
  Semantics::on_equality_expression(Expression* e1, 
                                     Expression* e2,
                                     const Token& op)
  {
    return nullptr;
  }

  Expression*
  Semantics::on_relational_expression(Expression* e1, 
                                       Expression* e2,
                                       const Token& op)
  {
    return nullptr;
  }

  Expression*
  Semantics::on_shift_expression(Expression* e1, 
                                  Expression* e2,
                                  const Token& op)
  {
    return nullptr;
  }

  Expression*
  Semantics::on_additive_expression(Expression* e1, 
                                     Expression* e2,
                                     const Token& op)
  {
    return nullptr;
  }

  Expression*
  Semantics::on_multiplicative_expression(Expression* e1, 
                                           Expression* e2,
                                           const Token& op)
  {
    return nullptr;
  }

  Expression*
  Semantics::on_cast_expression(Expression* e, 
                                Type_specifier* t)
  {
    return nullptr;
  }

  Expression*
  Semantics::on_unary_expression(Expression* e, 
                                 const Token& op)
  {
    switch (op.get_name()) {
    case Token::bang:
      return on_logical_not_expression(e, op);
    default:
      __builtin_unreachable();
    }
    return nullptr;
  }

  Expression*
  Semantics::on_call_expression(Expression* e, 
                                const Expression_seq& args,
                                const Token& lparen,
                                const Token& rparen)
  {
    return nullptr;
  }

  Expression*
  Semantics::on_subscript_expression(Expression* e, 
                                      const Expression_seq& args,
                                      const Token& lparen,
                                      const Token& rparen)
  {
    return nullptr;
  }

  Expression*
  Semantics::on_unit_literal(const Token& tok)
  {
    return nullptr;
  }

  Expression*
  Semantics::on_boolean_literal(const Token& tok)
  {
    Type* type = m_cxt.get_bool_type();
    bool val = tok.is(Token::true_kw);
    return new Bool_literal(type, tok, val);
  }
  
  Expression*
  Semantics::on_integer_literal(const Token& tok)
  {
    Type* type = m_cxt.get_int_type();
    int val = 13; // FIXME
    return new Int_literal(type, tok, val);
  }
  
  Expression*
  Semantics::on_float_literal(const Token& tok)
  {
    return nullptr;
  }
  
  Expression*
  Semantics::on_character_literal(const Token& tok)
  {
    return nullptr;
  }
  
  Expression*
  Semantics::on_string_literal(const Token& tok)
  {
    return nullptr;
  }

  Expression*
  Semantics::on_id_expression(const Token& id)
  {
    // Search for declarations with the given name.
    Declaration_set found = unqualified_lookup(id.get_symbol());
    if (found.is_empty()) {
      std::stringstream ss;
      ss << "no matching declaration for " << '\'' << id << '\'';
      throw std::runtime_error(ss.str());
    }
    Named_declaration* nd = found.get_single_declaration();
    
    // Id-expressions must be types.
    if (!nd->is_typed()) {
      std::stringstream ss;
      ss << "declaration " << '\'' << id << '\'' << " does not have a value";
      throw std::runtime_error(ss.str());
    }
    Typed_declaration* td = static_cast<Typed_declaration*>(nd);
    
    // Analyze the declaration to determine the type of the expression.
    Type* type = td->get_type();
    if (td->is_variable())
      type = m_cxt.get_reference_type(type);
    
    return new Id_expression(type, td);
  }

  Expression*
  Semantics::on_paren_expression(Expression* e,
                                  const Token& lparen, 
                                  const Token& rparen)
  {
    return nullptr;
  }

} // namespace beaker
