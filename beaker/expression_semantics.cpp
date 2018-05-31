#include "semantics.hpp"
#include "expression.hpp"
#include "arithmetic_expression.hpp"
#include "bitwise_expression.hpp"
#include "logical_expression.hpp"
#include "relational_expression.hpp"
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
    e2 = convert_to_value(e2);
    e1 = require_reference_to(e1, e2->get_type());
    return new Assignment_expression(e1->get_type(), e1, e2, op);
  }

  /// The first operand shall be converted to bool. The second and third
  /// operands shall be converted to their common type. The type of the
  /// expression is that common type.
  Expression*
  Semantics::on_conditional_expression(Expression* e1, 
                                       Expression* e2, 
                                       Expression* e3,
                                       const Token& question,
                                       const Token& colon)
  {
    e1 = convert_to_bool(e1);
    std::tie(e2, e3) = convert_to_common_type(e2, e3);
    return new Conditional_expression(e2->get_type(), e1, e2, e3, question, colon);
  }

  /// The operands are converted to bool values.
  /// The type of the expression is bool.
  Expression*
  Semantics::on_logical_expression(Expression* e1, 
                                   Expression* e2,
                                   const Token& op)
  {
    e1 = convert_to_bool(e1);
    e2 = convert_to_bool(e2);
    switch (op.get_name()) {
    case Token::ampersand_ampersand:
      return new Logical_and_expression(m_cxt.get_bool_type(), e1, e2, op);
    case Token::bar_bar:    
      return new Logical_or_expression(m_cxt.get_bool_type(), e1, e2, op);
    default:
      break;
    }
    __builtin_unreachable();
  }

  /// The operand shall be converted to bool. The type of the expression is
  /// bool.
  Expression*
  Semantics::on_logical_expression(Expression* e,
                                   const Token& op)
  {
    e = convert_to_bool(e);
    return new Logical_not_expression(m_cxt.get_bool_type(), e, op);
  }

  /// The operands shall be converted to a common integer type. The type of
  /// the expression shall be that type.
  Expression*
  Semantics::on_bitwise_expression(Expression* e1, 
                                   Expression* e2,
                                   const Token& op)
  {
    std::tie(e1, e2) = require_common_integer(e1, e2);
    switch (op.get_name()) {
    case Token::ampersand:
      return new Bitwise_and_expression(e1->get_type(), e1, e2, op);
    case Token::bar:
      return new Bitwise_or_expression(e1->get_type(), e1, e2, op);
    case Token::caret:
      return new Bitwise_xor_expression(e1->get_type(), e1, e2, op);
    default:
      break;
    }
    __builtin_unreachable();
  }

  /// The operand shall have integer type. The type of the expression is that
  /// type.
  Expression*
  Semantics::on_bitwise_expression(Expression* e,
                                   const Token& op)
  {
    e = require_integer(e);
    return new Bitwise_not_expression(e->get_type(), e, op);
  }

  /// The operands shall be converted to their common value type. The type
  /// of the expression is bool.
  Expression*
  Semantics::on_equality_expression(Expression* e1, 
                                    Expression* e2,
                                    const Token& op)
  {
    std::tie(e1, e2) = convert_to_common_value(e1, e2);
    Type* t = m_cxt.get_bool_type();
    switch (op.get_name()) {
    case Token::equal_equal:
      return new Equal_to_expression(t, e1, e2, op);
    case Token::bang_equal:
      return new Not_equal_to_expression(t, e1, e2, op);
    default:
      break;
    }
    __builtin_unreachable();
  }

  /// The operands shall be converted to their common value type. The type
  /// of the expression is bool.
  Expression*
  Semantics::on_relational_expression(Expression* e1, 
                                      Expression* e2,
                                      const Token& op)
  {
    std::tie(e1, e2) = convert_to_common_value(e1, e2);
    Type* t = m_cxt.get_bool_type();
    switch (op.get_name()) {
    case Token::less:
      return new Less_than_expression(t, e1, e2, op);
    case Token::greater:
      return new Greater_than_expression(t, e1, e2, op);
    case Token::less_equal:
      return new Not_greater_than_expression(t, e1, e2, op);
    case Token::greater_equal:
      return new Not_less_than_expression(t, e1, e2, op);
    default:
      break;
    }
    __builtin_unreachable();
  }

  /// \todo The operands shall have integer type (but necessarily the same). 
  /// The result type is that of e1.
  Expression*
  Semantics::on_shift_expression(Expression* e1, 
                                 Expression* e2,
                                 const Token& op)
  {
    e1 = require_integer(e1);
    e2 = require_integer(e2);
    switch (op.get_name()) {
    case Token::less_less:
      return new Shift_left_expression(e1->get_type(), e1, e2, op);
    case Token::greater_greater:
      return new Shift_right_expression(e1->get_type(), e1, e2, op);
    default:
      break;
    }
    __builtin_unreachable();
  }

  /// \todo The operands shall be converted to a common value type. The result
  /// type shall be the common type.
  Expression*
  Semantics::on_additive_expression(Expression* e1, 
                                    Expression* e2,
                                    const Token& op)
  {
    std::tie(e1, e2) = convert_to_common_value(e1, e2);
    switch (op.get_name()) {
    case Token::plus:
      return new Addition_expression(e1->get_type(), e1, e2, op);
    case Token::minus:
      return new Subtraction_expression(e1->get_type(), e1, e2, op);
    default:
      break;
    }
    __builtin_unreachable();
  }

  /// The operand shall have integer type. The result type is that type.
  Expression*
  Semantics::on_additive_expression(Expression* e,
                                    const Token& op)
  {
    e = require_integer(e);
    return new Negation_expression(e->get_type(), e, op);
  }

  /// \todo The operands shall be converted to a common value type. The result
  /// type shall be the common type.
  Expression*
  Semantics::on_multiplicative_expression(Expression* e1, 
                                          Expression* e2,
                                          const Token& op)
  {
    std::tie(e1, e2) = convert_to_common_value(e1, e2);
    switch (op.get_name()) {
    case Token::star:
      return new Multiplication_expression(e1->get_type(), e1, e2, op);
    case Token::slash:
      return new Quotient_expression(e1->get_type(), e1, e2, op);
    case Token::percent:
      return new Remainder_expression(e1->get_type(), e1, e2, op);
    default:
      break;
    }
    __builtin_unreachable();
  }

  /// The operand shall have integer type. The result type is that type.
  Expression*
  Semantics::on_multiplicative_expression(Expression* e,
                                          const Token& op)
  {
    e = require_integer(e);
    return new Reciprocal_expression(e->get_type(), e, op);
  }

  Expression*
  Semantics::on_cast_expression(Expression* e, 
                                Type_specifier* t)
  {
    return nullptr;
  }

  /// Dispatches to the appropriate semantic action.
  Expression*
  Semantics::on_unary_expression(Expression* e, 
                                 const Token& op)
  {
    switch (op.get_name()) {
    case Token::bang:
      return on_logical_expression(e, op);
    case Token::tilde:
      return on_bitwise_expression(e, op);
    case Token::minus:
      return on_additive_expression(e, op);
    case Token::slash:
      return on_multiplicative_expression(e, op);
    default:
      break;
    }
    __builtin_unreachable();
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
    // Get the integer value from the token.
    char* end;
    std::intmax_t val = std::strtoll(tok.get_spelling(), &end, 10);
    assert(*end == 0);

    Type* type = m_cxt.get_int_type();
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

  /// The type of the expression is determined by the declaration denoted by
  /// identifier. If the declaration is a variable or reference, the type
  /// is a reference to the declared type of the entity. Otherwise, the type
  /// is just the declared type of the entity.
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
    return make_id_expression(found.get_single_declaration());
  }

  Expression*
  Semantics::make_id_expression(Named_declaration* d)
  {
    // Unwrap references to parameters.
    if (Parameter* parm = dynamic_cast<Parameter*>(d))
      d = parm->get_declaration();

    // Id-expressions must be typed.
    if (!d->is_typed()) {
      std::stringstream ss;
      ss << "declaration " << '\'' << d->get_name() << '\'' << " does not have a value";
      throw std::runtime_error(ss.str());
    }
    
    return make_id_expression(static_cast<Typed_declaration*>(d));
  }

  Expression*
  Semantics::make_id_expression(Typed_declaration* d)
  {
    // Analyze the declaration to determine the type of the expression.
    //
    // FIXME: Do we really need to adjust for declarations?
    Type* type = d->get_type();
    if (d->is_variable() || d->is_reference())
      type = m_cxt.get_reference_type(type);

    return new Id_expression(type, d);
  }

  Expression*
  Semantics::make_init_expression(Variable_declaration* d)
  {
    Type* type = m_cxt.get_reference_type(d->get_type());
    return new Init_expression(type, d);
  }

  Expression*
  Semantics::on_paren_expression(Expression* e,
                                 const Token& lparen, 
                                 const Token& rparen)
  {
    return nullptr;
  }

} // namespace beaker
