#include "instruction_generation.hpp"
#include "function_generation.hpp"
#include "type.hpp"
#include "expression.hpp"
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

  void
  Instruction_generator::generate_expression(const Expression* e)
  {
  }

  void
  Instruction_generator::generate_statement(const Statement* s)
  {
  }

} // namespace beaker
