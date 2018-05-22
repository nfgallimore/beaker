#include "function_generation.hpp"
#include "module_generation.hpp"
#include "type.hpp"
#include "declaration.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>

namespace beaker
{
  Function_context::Function_context(Module_context& parent, const Function_declaration* fn)
    : m_parent(parent), m_fn(fn), m_llvm()
  { 
    // FIXME: Create the function.
  }

  void
  Function_context::generate()
  { }

  /// Returns the Beaker context.
  Context& 
  Function_context::get_beaker_context() const
  { 
    return m_parent.get_beaker_context(); 
  }

  /// Returns the LLVM context.
  llvm::LLVMContext* 
  Function_context::get_llvm_context() const
  { 
    return m_parent.get_llvm_context(); 
  }

} // namespace beaker
