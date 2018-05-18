#pragma once

#include <beaker/expression.hpp>

namespace beaker
{
  /// Represents bitwise and expressions.
  class Bitwise_and_expression : public Binary_operator
  {
  public:
    Bitwise_and_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(and_kind, t, lhs, rhs, op)
    { }
  };


  /// Represents bitwise inclusive expressions.
  class Bitwise_or_expression : public Binary_operator
  {
  public:
    Bitwise_or_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(or_kind, t, lhs, rhs, op)
    { }
  };


  /// Represents bitwise inclusive expressions.
  class Bitwise_xor_expression : public Binary_operator
  {
  public:
    Bitwise_xor_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(or_kind, t, lhs, rhs, op)
    { }
  };


  /// Represents bitwise not (one's complement) expressions.
  class Bitwise_not_expression : public Unary_operator
  {
  public:
    Bitwise_not_expression(Type* t, Expression* e, const Token& op)
      : Unary_operator(not_kind, t, e, op)
    { }
  };

} // namespace beaker
