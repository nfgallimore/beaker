#pragma once

#include <beaker/expression.hpp>

namespace beaker
{
  /// Represents addition expressions.
  class Addition_expression : public Binary_operator
  {
  public:
    Addition_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(and_kind, t, lhs, rhs, op)
    { }
  };

  /// Represents subtraction expressions.
  class Subtraction_expression : public Binary_operator
  {
  public:
    Subtraction_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(or_kind, t, lhs, rhs, op)
    { }
  };

  /// Represents multiplication expressions.
  class Multiplication_expression : public Binary_operator
  {
  public:
    Multiplication_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(or_kind, t, lhs, rhs, op)
    { }
  };

  /// Represents quotient computations.
  class Quotient_expression : public Binary_operator
  {
  public:
    Quotient_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(or_kind, t, lhs, rhs, op)
    { }
  };

  /// Represents remainder computations.
  class Remainder_expression : public Binary_operator
  {
  public:
    Remainder_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(or_kind, t, lhs, rhs, op)
    { }
  };

  /// Represents the computation of the additive inverse.
  class Negation_expression : public Unary_operator
  {
  public:
    Negation_expression(Type* t, Expression* e, const Token& op)
      : Unary_operator(not_kind, t, e, op)
    { }
  };

  /// Represents the computation of the multiplicative inverse.
  class Reciprocal_expression : public Unary_operator
  {
  public:
    Reciprocal_expression(Type* t, Expression* e, const Token& op)
      : Unary_operator(not_kind, t, e, op)
    { }
  };

} // namespace beaker
