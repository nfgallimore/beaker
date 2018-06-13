#pragma once

#include <beaker/expression.hpp>

namespace beaker
{
  /// Represents addition expressions.
  class Addition_expression : public Binary_operator
  {
  public:
    Addition_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(add_kind, t, lhs, rhs, op)
    { }
  };


  /// Represents subtraction expressions.
  class Subtraction_expression : public Binary_operator
  {
  public:
    Subtraction_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(sub_kind, t, lhs, rhs, op)
    { }
  };


  /// Represents the computation of the additive inverse.
  class Negation_expression : public Unary_operator
  {
  public:
    Negation_expression(Type* t, Expression* e, const Token& op)
      : Unary_operator(neg_kind, t, e, op)
    { }
  };


  /// Represents multiplication expressions.
  class Multiplication_expression : public Binary_operator
  {
  public:
    Multiplication_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(mul_kind, t, lhs, rhs, op)
    { }
  };

  /// Represents quotient computations. The operands of quotient expressions
  /// are euclidean rings (i.e., integers).
  class Quotient_expression : public Binary_operator
  {
  public:
    Quotient_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(quo_kind, t, lhs, rhs, op)
    { }
  };


  /// Represents remainder computations. The operands of quotient expressions
  /// are euclidean rings (i.e., integers).
  class Remainder_expression : public Binary_operator
  {
  public:
    Remainder_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(rem_kind, t, lhs, rhs, op)
    { }
  };


  /// Represents closed division. The operands of division are fields (e.g.,
  /// reals or floating point values).
  ///
  /// \todo Allow an alternative spelling for integer operands that computes
  /// the divmod operation?
  class Division_expression : public Binary_operator
  {
  public:
    Division_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(rem_kind, t, lhs, rhs, op)
    { }
  };


  /// Represents the computation of the multiplicative inverse.
  class Reciprocal_expression : public Unary_operator
  {
  public:
    Reciprocal_expression(Type* t, Expression* e, const Token& op)
      : Unary_operator(rec_kind, t, e, op)
    { }
  };

} // namespace beaker
