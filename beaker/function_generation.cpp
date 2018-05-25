#include "function_generation.hpp"
#include "module_generation.hpp"
#include "instruction_generation.hpp"
#include "type.hpp"
#include "expression.hpp"
#include "declaration.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>

#include <iostream>

namespace beaker
{
  Function_context::Function_context(Module_context& parent)
    : m_parent(parent), m_llvm()
  { }

  Function_context::Function_context(Module_context& parent, llvm::Function* fn)
    : m_parent(parent), m_llvm(fn)
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

  llvm::BasicBlock*
  Function_context::make_block(const char* label)
  {
    return llvm::BasicBlock::Create(*get_llvm_context(), label);
  }

  void
  Function_context::emit_block(llvm::BasicBlock* b)
  {
    b->insertInto(m_llvm);
    m_block = b;
  }

  void
  Function_context::declare(const Typed_declaration* d, llvm::Value* v)
  {
    assert(m_locals.count(d) == 0);
    m_locals.emplace(d, v);
  }

  llvm::Value*
  Function_context::lookup(const Typed_declaration* d)
  {
    auto iter = m_locals.find(d);
    if (iter != m_locals.end())
      return iter->second;
    return m_parent.lookup(d);
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
    assert(!m_llvm);

    llvm::Module* mod = get_llvm_module();
    std::string name = generate_external_name(d);

    // FIXME: Support internal and other forms of linkage.
    auto link = llvm::GlobalValue::ExternalLinkage;

    // Compute the type of the function.
    llvm::FunctionType* type = generate_function_type(d);

    // Create and declare the function.
    m_llvm = llvm::Function::Create(type, link, name, mod);
    m_parent.declare(d, m_llvm);

    // FIXME: Emit the definition.
    // Set up function arguments, initialize variable parms, create
    // the entry block, generate the definition.
  }
  
  void
  Function_context::generate_definition(const Expression* e)
  {
    // Establish the entry block.
    m_entry = m_block = make_block("entry");
    emit_block(m_entry);

    // Emit the definition.
    Instruction_generator gen(*this);
    llvm::Value* val = gen.generate_expression(e);

    // Create a return statemnet.
    //
    // FIXME: That e might not have a type is a bit of a hack. We need
    // to ensure that the type of the expression agrees with the type of
    // the function. Note that a null type is equivalent to a void type.
    llvm::IRBuilder<> ir(get_current_block());
    if (!e->get_type())
      ir.CreateRetVoid();
    else
      ir.CreateRet(val);
  }

} // namespace beaker
