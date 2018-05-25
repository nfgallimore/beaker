#include "variable_generation.hpp"
#include "module_generation.hpp"
#include "function_generation.hpp"
#include "type.hpp"
#include "initializer.hpp"
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
  Variable_context::Variable_context(Module_context& parent)
    : m_parent(parent), m_llvm()
  { }

  Context& 
  Variable_context::get_beaker_context() const
  { 
    return m_parent.get_beaker_context(); 
  }

  llvm::LLVMContext* 
  Variable_context::get_llvm_context() const
  { 
    return m_parent.get_llvm_context(); 
  }

  llvm::Module*
  Variable_context::get_llvm_module() const
  {
    return m_parent.get_llvm_module();
  }
  
  std::string
  Variable_context::generate_external_name(const Named_declaration* d)
  {
    return m_parent.generate_external_name(d);
  }

  llvm::Type*
  Variable_context::generate_type(const Type* t)
  {
    return m_parent.generate_type(t);
  }

  llvm::Type*
  Variable_context::generate_type(const Typed_declaration* d)
  {
    return m_parent.generate_type(d);
  }

  void
  Variable_context::generate(const Data_declaration* d)
  {
    llvm::Module* mod = get_llvm_module();
    std::string name = generate_external_name(d);

    // FIXME: Implement internal and other forms of linkage.
    auto link = llvm::GlobalValue::ExternalLinkage;

    // FIXME: Adjust the type based on the kind of declaration?
    llvm::Type* type = generate_type(d);

    // Create the initial initializer. We may overwrite this if the
    // declarations initializer is a constant expression.
    //
    // FIXME: This could be the result of a trial evaluation.
    llvm::Constant* init = llvm::Constant::getNullValue(type);

    // Create and declare the variable.
    m_llvm = new llvm::GlobalVariable(*mod, type, false, link, nullptr, name);
    m_parent.declare(d, m_llvm);

    // Generate the constructor.
    std::string cname = "__" + *d->get_name() + "_init";
    llvm::Function* ctor = m_parent.make_constructor(cname.c_str());
    Function_context fn(m_parent, ctor);
    fn.generate_definition(d->get_initializer());
  }

} // namespace beaker
