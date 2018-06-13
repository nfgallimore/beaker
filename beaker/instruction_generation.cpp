#include "instruction_generation.hpp"
#include "function_generation.hpp"

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

  Global_context&
  Instruction_generator::get_global_context()
  {
    return m_parent.get_global_context();
  }

  Module_context&
  Instruction_generator::get_module_context()
  {
    return m_parent.get_module_context();
  }

  llvm::BasicBlock*
  Instruction_generator::get_current_block() const
  {
    return m_parent.get_current_block();
  }

  llvm::BasicBlock*
  Instruction_generator::get_entry_block() const
  {
    return m_parent.get_entry_block();
  }

  llvm::BasicBlock*
  Instruction_generator::make_block(const char* label)
  {
    return m_parent.make_block(label);
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

} // namespace beaker
