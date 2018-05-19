#pragma once

#include <beaker/expression.hpp>

namespace beaker
{
  /// Represents expressions of the form `e1 & e2`.
  class Bitwise_and_expression : public Binary_operator
  {
  public:
    Bitwise_and_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(bit_and_kind, t, lhs, rhs, op)
    { }
  };


  /// Represents expressions of the form `e1 | e2`.
  class Bitwise_or_expression : public Binary_operator
  {
  public:
    Bitwise_or_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(bit_ior_kind, t, lhs, rhs, op)
    { }
  };


  /// Represents expressions of the form `e1 ^ e2`.
  class Bitwise_xor_expression : public Binary_operator
  {
  public:
    Bitwise_xor_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(bit_xor_kind, t, lhs, rhs, op)
    { }
  };


  /// Represents expressions of the form `~e`.
  class Bitwise_not_expression : public Unary_operator
  {
  public:
    Bitwise_not_expression(Type* t, Expression* e, const Token& op)
      : Unary_operator(bit_not_kind, t, e, op)
    { }
  };


  /// Represents expressions of the form `e1 << e2`.
  class Shift_left_expression : public Binary_operator
  {
  public:
    Shift_left_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(bit_shl_kind, t, lhs, rhs, op)
    { }
  };


  /// Represents expressions of the form `e1 >> e2`.
  class Shift_right_expression : public Binary_operator
  {
  public:
    Shift_right_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(bit_shr_kind, t, lhs, rhs, op)
    { }
  };

} // namespace beaker
