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
    : m_parent(parent), m_llvm()
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
  Module_context::declare(const Typed_declaration* d, llvm::GlobalValue* v)
  {
    assert(m_globals.count(d) == 0);
    m_globals.emplace(d, v);
  }

  llvm::GlobalValue*
  Module_context::lookup(const Typed_declaration* d)
  {
    assert(m_globals.count(d) != 0);
    return m_globals.find(d)->second;
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

  llvm::Constant*
  Module_context::generate_constant(const Type* t, const Value& v)
  {
    return m_parent.generate_constant(t, v);
  }

  llvm::Constant*
  Module_context::generate_constant(const Object* o)
  {
    return m_parent.generate_constant(o);
  }

  void
  Module_context::generate_module(const Translation_unit* d)
  { 
    assert(!m_llvm);

    // Create the module.
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
    auto link = llvm::Function::PrivateLinkage;
    std::string name = "__bkr_global_var_init_" + std::to_string(n) + "__";
    auto type = llvm::FunctionType::get(m_parent.get_llvm_void_type(), {}, false);
    auto fn = llvm::Function::Create(type, link, name, m_llvm);

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

    // Generate the global variable initialization function.
    auto link = llvm::Function::PrivateLinkage;
    std::string name = "__bkr_global_vars_init__";
    auto type = llvm::FunctionType::get(m_parent.get_llvm_void_type(), {}, false);
    auto fn = llvm::Function::Create(type, link, name, m_llvm);

    // Generate the definition as a sequence of calls to constructors.
    Function_context cxt(*this, fn);
    cxt.start_definition();
    for (llvm::Function* ctor : ctors) {
      llvm::IRBuilder<> ir(cxt.get_current_block());
      llvm::Value* c = ir.CreateCall(ctor, llvm::None);
    }
    cxt.finish_definition();

    {
      // Generate the @llvm.global.ctors variable.
      //
      // FIXME: Move this to a different function. And simplify.
      auto link = llvm::Function::AppendingLinkage;
      std::string name = "llvm.global.ctors";
      
      // Build the record type.
      llvm::Type* elem_types[] {
        m_parent.get_llvm_i32_type(), // i32
        ctors[0]->getType(), // void()*
        m_parent.get_llvm_i8_type()->getPointerTo() // i8*
      };
      llvm::StructType* elem_type = llvm::StructType::create(*get_llvm_context(), elem_types);

      /// Build the record.
      llvm::Constant* elems[] {
        m_parent.get_llvm_int(elem_types[0], 1 << 16),
        fn,
        m_parent.get_llvm_null(elem_types[2])
      };
      llvm::Constant* inits[1] { 
        llvm::ConstantStruct::get(elem_type, elems)
      };

      // Build the initializer type and value.
      llvm::ArrayType* init_type = llvm::ArrayType::get(elem_type, 1);
      llvm::Constant* init = llvm::ConstantArray::get(init_type, inits);
      
      auto var = new llvm::GlobalVariable(*m_llvm, type, false, link, init, name);
    }
  }

} // namespace beaker
