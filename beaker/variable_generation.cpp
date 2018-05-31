#include "variable_generation.hpp"
#include "function_generation.hpp"
#include "module_generation.hpp"
#include "global_generation.hpp"
#include "type.hpp"
#include "initializer.hpp"
#include "declaration.hpp"
#include "evaluation.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>
#include <sstream>

namespace beaker
{
  Variable_context::Variable_context(Module_context& parent)
    : m_parent(parent), m_llvm()
  { }

  Context& 
  Variable_context::get_beaker_context()
  { 
    return get_global_context().get_beaker_context(); 
  }

  llvm::LLVMContext* 
  Variable_context::get_llvm_context()
  { 
    return get_global_context().get_llvm_context(); 
  }

  llvm::Module*
  Variable_context::get_llvm_module()
  {
    return get_module_context().get_llvm_module();
  }

  Global_context&
  Variable_context::get_global_context()
  {
    return get_module_context().get_global_context();
  }
  
  std::string
  Variable_context::generate_external_name(const Named_declaration* d)
  {
    return get_global_context().generate_external_name(d);
  }

  llvm::Type*
  Variable_context::generate_type(const Type* t)
  {
    return get_global_context().generate_type(t);
  }

  llvm::Type*
  Variable_context::generate_type(const Typed_declaration* d)
  {
    return get_global_context().generate_type(d);
  }

  llvm::Constant*
  Variable_context::generate_constant(const Type* t, const Value& v)
  {
    return get_global_context().generate_constant(t, v);
  }

  llvm::Constant*
  Variable_context::generate_constant(const Object* o)
  {
    return get_global_context().generate_constant(o);
  }

  void
  Variable_context::generate(const Data_declaration* d)
  {
    switch (d->get_kind()) {
    case Declaration::var_kind:
      return generate_variable(static_cast<const Variable_declaration*>(d));
    case Declaration::val_kind:
      return generate_value(static_cast<const Value_declaration*>(d));
    case Declaration::ref_kind:
      return generate_reference(static_cast<const Reference_declaration*>(d));
    default:
      break;
    }
    assert(false);
  }

  void
  Variable_context::generate_variable(const Variable_declaration* d)
  {
    llvm::Module* mod = get_llvm_module();
    std::string name = generate_external_name(d);

    // FIXME: Implement internal and other forms of linkage.
    auto link = llvm::GlobalValue::ExternalLinkage;

    // FIXME: Adjust the type based on the kind of declaration?
    llvm::Type* type = generate_type(d);

    // Create and declare the variable.
    m_llvm = new llvm::GlobalVariable(*mod, type, false, link, nullptr, name);
    get_module_context().declare(d, m_llvm);

    // Generate the static initializer.
    llvm::Constant* init = generate_static_initializer(d);
    m_llvm->setInitializer(init);

    // FIXME: If the variable has a non-trivial destructor, then we need
    // to generate a dynamic finalizer for the object. This isn't something
    // to worry about until we have classes, however.
  }

  void
  Variable_context::generate_value(const Value_declaration* d)
  {
    // Evaluate the initializer as a constant and associate with the 
    // declaration. Note that no storage is associated with the value.
    Value v;
    try {
      v = evaluate(get_beaker_context(), d->get_initializer());
    }
    catch (std::runtime_error& err) {
      std::stringstream ss;
      ss << "cannot initialize constant " << '(' << err.what() << ')';
      throw std::runtime_error(ss.str());
    }
    llvm::Constant* c = generate_constant(d->get_type(), v);
    get_module_context().declare(d, c);
  }

  // FIXME: Can we clean this up a bit?
  void
  Variable_context::generate_reference(const Reference_declaration* d)
  {
    // Evaluate the initializer as a constant and associate with the 
    // declaration. Note that no storage is associated with the value.
    Value val;
    try {
      val = evaluate(get_beaker_context(), d->get_initializer());
    }
    catch (std::runtime_error& err) {
      std::stringstream ss;
      ss << "cannot initialize constant " << '(' << err.what() << ')';
      throw std::runtime_error(ss.str());
    }

    // The value of the reference is the address of the corresponding
    // global declaration (or globally allocated object).
    llvm::Constant* c;
    Object* obj = val.get_reference();
    Creator creator = obj->get_creator();
    if (creator.is_declaration())
      c = get_module_context().lookup(creator.get_declaration());
    else
      assert(false); // Not implemented.

    get_module_context().declare(d, c);
  }

  llvm::Constant*
  Variable_context::generate_static_initializer(const Variable_declaration* d)
  {
    if (llvm::Constant* init = generate_constant_initializer(d))
      return init;

    // Request the construction of a dynamic constructor.
    assert(d->is_variable());
    const auto* var = static_cast<const Variable_declaration*>(d);
    get_module_context().add_constructor(var);
    
    // The static object is zero-initialized.
    return generate_zero_initializer(d);
  }

  llvm::Constant*
  Variable_context::generate_constant_initializer(const Variable_declaration* d)
  {
    llvm::Constant* init;
    try {
      // Elaborate the declaration in order to generate a constant.
      Constant_evaluator eval(get_beaker_context());
      eval.elaborate(d);
      Value val = eval.fetch(d);

      // For variables, generate the constant from the stored value. Otherwise,
      // the value is just the associated constant.
      if (const auto* var = dynamic_cast<const Variable_declaration*>(d))
        init = generate_constant(val.get_reference());
      else
        init = generate_constant(d->get_type(), val);
    }
    catch (std::runtime_error& err) {
      init = nullptr;
    }
    return init;
  }

  llvm::Constant*
  Variable_context::generate_zero_initializer(const Variable_declaration* d)
  {
    return get_global_context().get_llvm_zero(d->get_type());
  }

} // namespace beaker
