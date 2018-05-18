#pragma once

#include <beaker/expression.hpp>

namespace beaker
{
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
