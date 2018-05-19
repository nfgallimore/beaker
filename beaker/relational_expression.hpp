#pragma once

#include <beaker/expression.hpp>

namespace beaker
{
  /// Represents expressions of the form `e1 == e2`.
  class Equal_to_expression : public Binary_operator
  {
  public:
    Equal_to_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(eq_kind, t, lhs, rhs, op)
    { }
  };


  /// Represents expressions of the form `e1 != e2`.
  class Not_equal_to_expression : public Binary_operator
  {
  public:
    Not_equal_to_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(ne_kind, t, lhs, rhs, op)
    { }
  };


  /// Represents expressions of the form `e1 < e2`.
  class Less_than_expression : public Binary_operator
  {
  public:
    Less_than_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(lt_kind, t, lhs, rhs, op)
    { }
  };


  /// Represents expressions of the form `e1 > e2`.
  class Greater_than_expression : public Binary_operator
  {
  public:
    Greater_than_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(gt_kind, t, lhs, rhs, op)
    { }
  };


  /// Represents expressions of the form `e1 <= e2`.
  class Not_greater_than_expression : public Binary_operator
  {
  public:
    Not_greater_than_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(ng_kind, t, lhs, rhs, op)
    { }
  };


  /// Represents expressions of the form `e1 >= e2`.
  class Not_less_than_expression : public Binary_operator
  {
  public:
    Not_less_than_expression(Type* t, Expression* lhs, Expression* rhs, const Token& op)
      : Binary_operator(nl_kind, t, lhs, rhs, op)
    { }
  };


} // namespace beaker
