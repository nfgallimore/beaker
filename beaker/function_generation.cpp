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
  Function_context::Function_context(Module_context& parent)
    : m_parent(parent), m_llvm()
  { }

  Context& 
  Function_context::get_beaker_context() const
  { 
    return m_parent.get_beaker_context(); 
  }

  llvm::LLVMContext* 
  Function_context::get_llvm_context() const
  { 
    return m_parent.get_llvm_context(); 
  }

  llvm::Module*
  Function_context::get_llvm_module() const
  {
    return m_parent.get_llvm_module();
  }

  void
  Function_context::emit_block(llvm::BasicBlock* b)
  {
    b->insertInto(m_llvm);
    m_block = b;
  }

  std::string
  Function_context::generate_external_name(const Named_declaration* d)
  {
    return m_parent.generate_external_name(d);
  }

  llvm::Type*
  Function_context::generate_type(const Type* t)
  {
    return m_parent.generate_type(t);
  }

  llvm::Type*
  Function_context::generate_type(const Typed_declaration* d)
  {
    return m_parent.generate_type(d);
  }

  llvm::FunctionType*
  Function_context::generate_function_type(const Function_declaration* d)
  {
    llvm::Type* t = generate_type(d);
    llvm::Type* f = t->getPointerElementType();
    return llvm::cast<llvm::FunctionType>(f);
  }

  void
  Function_context::generate(const Function_declaration* d)
  {
    llvm::Module* mod = get_llvm_module();
    std::string name = generate_external_name(d);

    // FIXME: Support internal and other forms of linkage.
    auto link = llvm::GlobalValue::ExternalLinkage;

    // Compute the type of the function.
    llvm::FunctionType* type = generate_function_type(d);

    // Create the function.
    m_llvm = llvm::Function::Create(type, link, name, mod);

    // FIXME: Emit the definition.
    // Set up function arguments, initialize variable parms, create
    // the entry block, generate the definition.
  }

} // namespace beaker
