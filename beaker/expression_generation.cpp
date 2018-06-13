#include "instruction_generation.hpp"
#include "expression.hpp"
#include "arithmetic_expression.hpp"
#include "relational_expression.hpp"
#include "logical_expression.hpp"
#include "conversion.hpp"
#include "initializer.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>

namespace beaker
{
  llvm::Value*
  Instruction_generator::generate_expression(const Expression* e)
  {
    switch (e->get_kind()) {
    case Expression::bool_kind:
      return generate_bool_literal(static_cast<const Bool_literal*>(e));
    
    case Expression::int_kind:
      return generate_int_literal(static_cast<const Int_literal*>(e));
    
    case Expression::id_kind:
    case Expression::init_kind:
      return generate_id_expression(static_cast<const Id_expression*>(e));

    // arithmetic expressions
    case Expression::add_kind:
      return generate_addition_expression(static_cast<const Addition_expression*>(e));
    case Expression::sub_kind:
      return generate_subtraction_expression(static_cast<const Subtraction_expression*>(e));
    case Expression::neg_kind:
      return generate_negation_expression(static_cast<const Negation_expression*>(e));
    case Expression::mul_kind:
      return generate_multiplication_expression(static_cast<const Multiplication_expression*>(e));
    case Expression::quo_kind:
      return generate_quotient_expression(static_cast<const Quotient_expression*>(e));
    case Expression::rem_kind:
      return generate_remainder_expression(static_cast<const Remainder_expression*>(e));
    case Expression::div_kind:
      return generate_division_expression(static_cast<const Division_expression*>(e));
    case Expression::rec_kind:
      return generate_reciprocal_expression(static_cast<const Reciprocal_expression*>(e));

    // bitwise expressions
    case Expression::bit_and_kind:
    case Expression::bit_ior_kind:
    case Expression::bit_xor_kind:
    case Expression::bit_not_kind:
    case Expression::bit_shl_kind:
    case Expression::bit_shr_kind:

    // logical expressions
    case Expression::cond_kind:
    case Expression::and_kind:
    case Expression::or_kind:
    case Expression::not_kind:
      break;

    // relational expressions
    case Expression::eq_kind:
      return generate_equal_to_expression(static_cast<const Equal_to_expression*>(e));
    case Expression::ne_kind:
      return generate_not_equal_to_expression(static_cast<const Not_equal_to_expression*>(e));
    case Expression::lt_kind:
      return generate_less_than_expression(static_cast<const Less_than_expression*>(e));
    case Expression::nl_kind:
      return generate_not_less_than_expression(static_cast<const Not_less_than_expression*>(e));
    case Expression::gt_kind:
      return generate_greater_than_expression(static_cast<const Greater_than_expression*>(e));
    case Expression::ng_kind:
      return generate_not_greater_than_expression(static_cast<const Not_greater_than_expression*>(e));

    // object expressions
    case Expression::assign_kind:
      return generate_assignment_expression(static_cast<const Assignment_expression*>(e));

    // conversions
    case Expression::imp_conv:
      return generate_implicit_conversion(static_cast<const Implicit_conversion*>(e));
      
    // initializers
    case Expression::empty_init:
    case Expression::def_init:
      break;
    case Expression::val_init:
      return generate_value_initialization(static_cast<const Value_initializer*>(e));
    }
    e->dump();
    assert(false);
  }

  llvm::Value*
  Instruction_generator::generate_bool_literal(const Bool_literal* e)
  {
    /// FIXME: Replace these with calls to the global context.
    if (e->get_value())
      return llvm::ConstantInt::getTrue(*get_llvm_context());
    else
      return llvm::ConstantInt::getFalse(*get_llvm_context());
  }

  llvm::Value*
  Instruction_generator::generate_int_literal(const Int_literal* e)
  {
    llvm::Type* type = generate_type(e->get_type());
    return llvm::ConstantInt::get(type, e->get_value());
  }

  llvm::Value*
  Instruction_generator::generate_id_expression(const Id_expression* e)
  {
    return lookup(e->get_declaration());
  }

  // Arithmetic expressions

  // FIXME: Handle unsigned and floating point expressions.
  llvm::Value*
  Instruction_generator::generate_addition_expression(const Addition_expression* e)
  {
    llvm::Value* lhs = generate_expression(e->get_lhs());
    llvm::Value* rhs = generate_expression(e->get_rhs());
    llvm::IRBuilder<> ir(get_current_block());
    return ir.CreateNSWAdd(lhs, rhs);
  }

  // FIXME: Handle unsigned and floating point expressions.
  llvm::Value*
  Instruction_generator::generate_subtraction_expression(const Subtraction_expression* e)
  {
    llvm::Value* lhs = generate_expression(e->get_lhs());
    llvm::Value* rhs = generate_expression(e->get_rhs());
    llvm::IRBuilder<> ir(get_current_block());
    return ir.CreateNSWSub(lhs, rhs);
  }

  // FIXME: Implement me.
  llvm::Value*
  Instruction_generator::generate_negation_expression(const Negation_expression* e)
  {
    assert(false);
  }

  // FIXME: Handle unsigned and floating point expressions.
  llvm::Value*
  Instruction_generator::generate_multiplication_expression(const Multiplication_expression* e)
  {
    llvm::Value* lhs = generate_expression(e->get_lhs());
    llvm::Value* rhs = generate_expression(e->get_rhs());
    llvm::IRBuilder<> ir(get_current_block());
    return ir.CreateNSWMul(lhs, rhs);
  }

  // FIXME: Handle unsigned expressions.
  llvm::Value*
  Instruction_generator::generate_quotient_expression(const Quotient_expression* e)
  {
    llvm::Value* lhs = generate_expression(e->get_lhs());
    llvm::Value* rhs = generate_expression(e->get_rhs());
    llvm::IRBuilder<> ir(get_current_block());
    return ir.CreateSDiv(lhs, rhs);
  }

  // FIXME: Handle unsigned expressions.
  llvm::Value*
  Instruction_generator::generate_remainder_expression(const Remainder_expression* e)
  {
    llvm::Value* lhs = generate_expression(e->get_lhs());
    llvm::Value* rhs = generate_expression(e->get_rhs());
    llvm::IRBuilder<> ir(get_current_block());
    return ir.CreateSRem(lhs, rhs);
  }

  llvm::Value*
  Instruction_generator::generate_division_expression(const Division_expression* e)
  {
    llvm::Value* lhs = generate_expression(e->get_lhs());
    llvm::Value* rhs = generate_expression(e->get_rhs());
    llvm::IRBuilder<> ir(get_current_block());
    return ir.CreateFDiv(lhs, rhs);
  }

  // FIXME: Implement me.
  llvm::Value*
  Instruction_generator::generate_reciprocal_expression(const Reciprocal_expression* e)
  {
    assert(false);
  }

  // Relational expressions

  /// \todo Handle floating point types.
  llvm::Value*
  Instruction_generator::generate_equal_to_expression(const Equal_to_expression* e)
  {
    llvm::Value* lhs = generate_expression(e->get_lhs());
    llvm::Value* rhs = generate_expression(e->get_rhs());
    llvm::IRBuilder<> ir(get_current_block());
    return ir.CreateICmpEQ(lhs, rhs);
  }
  
  /// \todo Handle floating point types.
  llvm::Value*
  Instruction_generator::generate_not_equal_to_expression(const Not_equal_to_expression* e)
  {
    llvm::Value* lhs = generate_expression(e->get_lhs());
    llvm::Value* rhs = generate_expression(e->get_rhs());
    llvm::IRBuilder<> ir(get_current_block());
    return ir.CreateICmpNE(lhs, rhs);
  }
  
  /// \todo Handle unsigned and floating point types.
  llvm::Value*
  Instruction_generator::generate_less_than_expression(const Less_than_expression* e)
  {
    llvm::Value* lhs = generate_expression(e->get_lhs());
    llvm::Value* rhs = generate_expression(e->get_rhs());
    llvm::IRBuilder<> ir(get_current_block());
    return ir.CreateICmpSLT(lhs, rhs);
  }
  
  /// \todo Handle unsigned and floating point types.
  llvm::Value*
  Instruction_generator::generate_not_less_than_expression(const Not_less_than_expression* e)
  {
    llvm::Value* lhs = generate_expression(e->get_lhs());
    llvm::Value* rhs = generate_expression(e->get_rhs());
    llvm::IRBuilder<> ir(get_current_block());
    return ir.CreateICmpSGE(lhs, rhs);
  }
  
  /// \todo Handle unsigned and floating point types.
  llvm::Value*
  Instruction_generator::generate_greater_than_expression(const Greater_than_expression* e)
  {
    llvm::Value* lhs = generate_expression(e->get_lhs());
    llvm::Value* rhs = generate_expression(e->get_rhs());
    llvm::IRBuilder<> ir(get_current_block());
    return ir.CreateICmpSGT(lhs, rhs);
  }
  
  /// \todo Handle unsigned and floating point types.
  llvm::Value*
  Instruction_generator::generate_not_greater_than_expression(const Not_greater_than_expression* e)
  {
    llvm::Value* lhs = generate_expression(e->get_lhs());
    llvm::Value* rhs = generate_expression(e->get_rhs());
    llvm::IRBuilder<> ir(get_current_block());
    return ir.CreateICmpSLE(lhs, rhs);
  }

  llvm::Value*
  Instruction_generator::generate_assignment_expression(const Assignment_expression* e)
  {
    llvm::Value* lhs = generate_expression(e->get_lhs());
    llvm::Value* rhs = generate_expression(e->get_rhs());
    llvm::IRBuilder<> ir(get_current_block());
    return ir.CreateStore(rhs, lhs);
  }

  llvm::Value*
  Instruction_generator::generate_implicit_conversion(const Implicit_conversion* e)
  {
    switch (e->get_conversion_kind()) {
    case Conversion::value_conv:
      return generate_value_conversion(e);
    case Conversion::bool_conv:
    case Conversion::int_prom:
    case Conversion::sign_ext:
    case Conversion::zero_ext:
    case Conversion::int_trunc:
    case Conversion::float_prom:
    case Conversion::float_dem:
    case Conversion::float_ext:
    case Conversion::float_trunc:
      break;
    }
    assert(false);
  }

  // A reference-to-value conversion is simply a load of the operand.
  llvm::Value*
  Instruction_generator::generate_value_conversion(const Conversion* e)
  {
    llvm::Value* ref = generate_expression(e->get_source());
    llvm::IRBuilder<> ir(get_current_block());
    return ir.CreateLoad(ref);
  }

  llvm::Value*
  Instruction_generator::generate_value_initialization(const Value_initializer* e)
  {
    llvm::Value* ref = generate_expression(e->get_object());
    llvm::Value* val = generate_expression(e->get_value());

    llvm::IRBuilder<> ir(get_current_block());
    return ir.CreateStore(val, ref);
  }

} // namespace beaker
