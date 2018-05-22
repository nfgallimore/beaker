#include "module_generation.hpp"
#include "global_generation.hpp"
#include "type.hpp"
#include "declaration.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>

namespace beaker
{
  Module_context::Module_context(Global_context& parent, const Translation_unit* tu)
    : m_parent(parent), 
      m_tu(tu), 
      m_llvm(new llvm::Module("a.ll", *get_llvm_context()))
  { }

  void
  Module_context::generate()
  { }

  Context& 
  Module_context::get_beaker_context() const
  { 
    return m_parent.get_beaker_context(); 
  }

  llvm::LLVMContext* 
  Module_context::get_llvm_context() const
  {
    return m_parent.get_llvm_context(); 
  }

} // namespace beaker
