#include "instruction_generation.hpp"
#include "function_generation.hpp"
#include "type.hpp"
#include "expression.hpp"
#include "conversion.hpp"
#include "initializer.hpp"
#include "statement.hpp"
#include "declaration.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>

namespace beaker
{
  Instruction_generator::Instruction_generator(Function_context& parent)
    : m_parent(parent)
  { }

  Context& 
  Instruction_generator::get_beaker_context() const
  { 
    return m_parent.get_beaker_context(); 
  }

  llvm::LLVMContext* 
  Instruction_generator::get_llvm_context() const
  { 
    return m_parent.get_llvm_context(); 
  }

  llvm::Module*
  Instruction_generator::get_llvm_module() const
  {
    return m_parent.get_llvm_module();
  }
  
  llvm::BasicBlock*
  Instruction_generator::get_current_block() const
  {
    return m_parent.get_current_block();
  }

  void
  Instruction_generator::emit_block(llvm::BasicBlock* b)
  {
    m_parent.emit_block(b);
  }

  void
  Instruction_generator::declare(const Typed_declaration* d, llvm::Value* v)
  {
    return m_parent.declare(d, v);
  }

  llvm::Value*
  Instruction_generator::lookup(const Typed_declaration* d)
  {
    return m_parent.lookup(d);
  }
  
  llvm::Type*
  Instruction_generator::generate_type(const Type* t)
  {
    return m_parent.generate_type(t);
  }

  llvm::Type*
  Instruction_generator::generate_type(const Typed_declaration* d)
  {
    return m_parent.generate_type(d);
  }

  llvm::Value*
  Instruction_generator::generate_expression(const Expression* e)
  {
    switch (e->get_kind()) {
    case Expression::bool_kind:
      return generate_bool_literal(static_cast<const Bool_literal*>(e));
    case Expression::int_kind:
      return generate_int_literal(static_cast<const Int_literal*>(e));
    case Expression::id_kind:
      return generate_id_expression(static_cast<const Id_expression*>(e));

    // arithmetic expressions
    case Expression::add_kind:
    case Expression::sub_kind:
    case Expression::mul_kind:
    case Expression::quo_kind:
    case Expression::rem_kind:
    case Expression::neg_kind:
    case Expression::rec_kind:

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

    // relational expressions
    case Expression::eq_kind:
    case Expression::ne_kind:
    case Expression::lt_kind:
    case Expression::gt_kind:
    case Expression::ng_kind:
    case Expression::nl_kind:

    // object expressions
    case Expression::assign_kind:

    // conversions
    case Expression::imp_conv:
      break;
      
    // initializers
    case Expression::empty_init:
    case Expression::def_init:
      break;
    case Expression::val_init:
      return generate_value_init(static_cast<const Value_initializer*>(e));
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

  llvm::Value*
  Instruction_generator::generate_value_init(const Value_initializer* e)
  {
    llvm::Value* ref = generate_expression(e->get_object());
    llvm::Value* val = generate_expression(e->get_value());

    llvm::IRBuilder<> ir(get_current_block());
    return ir.CreateStore(val, ref);
  }


  void
  Instruction_generator::generate_statement(const Statement* s)
  {
  }

} // namespace beaker
