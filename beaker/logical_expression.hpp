#pragma once

#include <beaker/expression.hpp>

namespace beaker
{
  /// Represents conditional expressions.
  class Conditional_expression : public Ternary_operator
  {
  public:
    Conditional_expression(Type* t, 
                           Expression* e1, 
                           Expression* e2, 
                           Expression* e3, 
                           const Token& tok1, 
                           const Token& tok2)
      : Ternary_operator(cond_kind, t, e1, e2, e3, tok1, tok2)
    { }

    /// Returns the condition of the expression.
    Expression* get_condition() const { return get_first(); }

    /// Returns the value when the condition is true.
    Expression* get_true_value() const { return get_second(); }

    /// Returns the value when the condition is false.
    Expression* get_false_value() const { return get_third(); }
  };


  /// Represents logical and expressions.
  class Logical_and_expression : public Binary_operator
  {
  public:
    Logical_and_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(and_kind, t, lhs, rhs, op)
    { }
  };


  /// Represents logical and expressions.
  class Logical_or_expression : public Binary_operator
  {
  public:
    Logical_or_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(or_kind, t, lhs, rhs, op)
    { }
  };


  /// Represents logical not expressions.
  class Logical_not_expression : public Unary_operator
  {
  public:
    Logical_not_expression(Type* t, Expression* e, const Token& op)
      : Unary_operator(not_kind, t, e, op)
    { }
  };

} // namespace beaker
