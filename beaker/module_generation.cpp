#include "module_generation.hpp"
#include "global_generation.hpp"
#include "function_generation.hpp"
#include "variable_generation.hpp"
#include "type.hpp"
#include "declaration.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/Support/raw_ostream.h>

namespace beaker
{
  Module_context::Module_context(Global_context& parent)
    : m_parent(parent)
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

  std::string
  Module_context::generate_external_name(const Named_declaration* d)
  {
    return m_parent.generate_external_name(d);
  }

  llvm::Type*
  Module_context::generate_type(const Type* t)
  {
    return m_parent.generate_type(t);
  }

  llvm::Type*
  Module_context::generate_type(const Typed_declaration* d)
  {
    return m_parent.generate_type(d);
  }

  void
  Module_context::generate_module(const Translation_unit* d)
  { 
    // Create the module.
    m_llvm = new llvm::Module("a.ll", *get_llvm_context());
    
    // Generate top-level declarations.
    for (const Declaration* tld : d->get_declarations())
      generate_global(tld);
    
    // FIXME: Actually write the module into an output file.
    llvm::outs() << *m_llvm;
  }

  void
  Module_context::generate_global(const Declaration* d)
  {
    switch (d->get_kind()) {
    case Declaration::val_kind:
    case Declaration::var_kind:
    case Declaration::ref_kind:
      return generate_variable(static_cast<const Data_declaration*>(d));

    case Declaration::func_kind:
      return generate_function(static_cast<const Function_declaration*>(d));
    
    default:
      break;
    }
    __builtin_unreachable();
  }

  void
  Module_context::generate_variable(const Data_declaration* d)
  {
    Variable_context var(*this);
    var.generate(d);
  }

  void
  Module_context::generate_function(const Function_declaration* d)
  {
    Function_context fn(*this);
    fn.generate(d);
  }


} // namespace beaker
