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
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>

namespace beaker
{
  Module_context::Module_context(Global_context& parent)
    : cg::Factory(parent.get_llvm_context()), m_parent(parent), m_llvm()
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

  void
  Module_context::declare(const Typed_declaration* d, llvm::Constant* c)
  {
    assert(m_globals.count(d) == 0);
    m_globals.emplace(d, c);
  }

  llvm::Constant*
  Module_context::lookup(const Typed_declaration* d)
  {
    assert(m_globals.count(d) != 0);
    return m_globals.find(d)->second;
  }

  llvm::Function*
  Module_context::make_external_function(const std::string& name, 
                                         llvm::FunctionType* type)
  {
    auto link = llvm::Function::ExternalLinkage;
    return llvm::Function::Create(type, link, name, m_llvm);
  }

  llvm::Function*
  Module_context::make_internal_function(const std::string& name,
                                         llvm::FunctionType* type)
  {
    auto link = llvm::Function::InternalLinkage;
    return llvm::Function::Create(type, link, name, m_llvm);
  }

  llvm::GlobalVariable*
  Module_context::make_appending_variable(const std::string& name, 
                                          llvm::Type* type)
  {
    return make_appending_variable(name, type, nullptr);
  }

  llvm::GlobalVariable*
  Module_context::make_appending_variable(const std::string& name, 
                                          llvm::Type* type,
                                          llvm::Constant* init)
  {
    auto link = llvm::GlobalVariable::AppendingLinkage;
    return new llvm::GlobalVariable(*m_llvm, type, false, link, init, name);
  }

  llvm::GlobalVariable*
  Module_context::make_llvm_global_ctors(llvm::Constant* init)
  {
    std::string name = "llvm.global_ctors";
    llvm::Type* type = init->getType();
    return make_appending_variable(name, type, init);
  }

  llvm::GlobalVariable*
  Module_context::make_llvm_global_dtors(llvm::Constant* init)
  {
    std::string name = "llvm.global_dtors";
    llvm::Type* type = init->getType();
    return make_appending_variable(name, type, init);
  }

  llvm::Function*
  Module_context::get_trap_intrinsic()
  {
    return llvm::Intrinsic::getDeclaration(m_llvm, llvm::Intrinsic::trap);
  }

  llvm::Function*
  Module_context::get_debugtrap_intrinsic()
  {
    return llvm::Intrinsic::getDeclaration(m_llvm, llvm::Intrinsic::debugtrap);
  }

  std::string
  Module_context::generate_external_name(const Named_declaration* d)
  {
    return get_global_context().generate_external_name(d);
  }

  llvm::Type*
  Module_context::generate_type(const Type* t)
  {
    return get_global_context().generate_type(t);
  }

  llvm::Type*
  Module_context::generate_type(const Typed_declaration* d)
  {
    return get_global_context().generate_type(d);
  }

  llvm::Constant*
  Module_context::generate_constant(const Type* t, const Value& v)
  {
    return get_global_context().generate_constant(t, v);
  }

  llvm::Constant*
  Module_context::generate_constant(const Object* o)
  {
    return get_global_context().generate_constant(o);
  }

  void
  Module_context::generate_module(const Translation_unit* d)
  { 
    assert(!m_llvm);

    // Create the module.
    //
    // FIXME: The name of the output file depends on configuration.
    m_llvm = new llvm::Module("a.ll", *get_llvm_context());
    
    // Generate top-level declarations.
    for (const Declaration* tld : d->get_declarations())
      generate_global(tld);

    // Generate global ctors and dtors.
    generate_constructors();
    
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

  void
  Module_context::add_constructor(const Variable_declaration* d)
  {
    m_ctors.push_back(d);
  }

  llvm::Function*
  Module_context::generate_constructor(const Variable_declaration* d, std::size_t n)
  {
    // Generate the declaration. Note that the name is based on index and
    // not the name of the declaration.
    std::string name = "__bkr_global_var_init_" + std::to_string(n) + "__";
    llvm::FunctionType* type = get_llvm_void_function_type();
    llvm::Function* fn = make_internal_function(name, type);

    // Generate the function definition.
    Function_context cxt(*this, fn);
    cxt.generate_definition(d->get_initializer());

    return fn;
  }

  void
  Module_context::generate_constructors()
  {
    if (m_ctors.empty())
      return;

    // Generate the constructors.
    std::vector<llvm::Function*> ctors;
    ctors.reserve(m_ctors.size());
    for (const Variable_declaration* d : m_ctors) {
      llvm::Function* ctor = generate_constructor(d, ctors.size());
      ctors.push_back(ctor);
    }

    // FIXME: Are there any other constructors to initialize?

    // Generate the global variable initialization function.
    std::string name = "__bkr_global_vars_init__";
    llvm::FunctionType* type = get_llvm_void_function_type();
    llvm::Function* fn = make_internal_function(name, type);

    // Generate the definition as a sequence of calls to constructors.
    Function_context cxt(*this, fn);
    cxt.start_definition();
    llvm::IRBuilder<> ir(cxt.get_current_block());
    for (llvm::Function* ctor : ctors) {
      ir.CreateCall(ctor, llvm::None);
    }
    ir.CreateRetVoid();
    cxt.finish_definition();

    /// Build the initializer for the global constructors.
    llvm::Constant* elems[] {
      get_llvm_int(get_llvm_i32_type(), 1 << 16),
      fn,
      get_llvm_null_byte_pointer()
    };
    llvm::Constant* inits[] { 
      get_llvm_struct(get_llvm_global_xtor_type(), elems)
    };
    llvm::Constant* init = get_llvm_array(inits);
    make_llvm_global_ctors(init);
  }

} // namespace beaker
