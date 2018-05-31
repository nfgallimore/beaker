#include "global_generation.hpp"
#include "type.hpp"
#include "declaration.hpp"
#include "value.hpp"
#include "object.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Constants.h>

namespace beaker
{
  Global_context::Global_context(Context& cxt)
    : m_cxt(cxt), m_llvm(new llvm::LLVMContext())
  { }

  Global_context::~Global_context()
  { }

  llvm::Type*
  Global_context::get_llvm_void_type() const
  {
    return llvm::Type::getVoidTy(*m_llvm);
  }

  llvm::Type*
  Global_context::get_llvm_int_type(int n) const
  {
    return llvm::Type::getIntNTy(*m_llvm, n);
  }

  llvm::Constant*
  Global_context::get_llvm_true()
  {
    return llvm::ConstantInt::getTrue(*m_llvm);
  }

  llvm::Constant*
  Global_context::get_llvm_false()
  {
    return llvm::ConstantInt::getFalse(*m_llvm);
  }

  llvm::Constant*
  Global_context::get_llvm_int(const Type* t, std::intmax_t n)
  {
    return get_llvm_int(generate_type(t), n);
  }

  llvm::Constant*
  Global_context::get_llvm_int(llvm::Type* t, std::intmax_t n) const
  {
    // FIXME: Handle unsigned integer constants.
    return llvm::ConstantInt::getSigned(t, n);    
  }

  llvm::Constant*
  Global_context::get_llvm_float(const Type* t, double n)
  {
    llvm::Type* type = generate_type(t);
    return llvm::ConstantFP::get(type, n);
  }

  llvm::Constant*
  Global_context::get_llvm_null(llvm::Type* t) const
  {
    return llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(t)); 
  }

  llvm::Constant*
  Global_context::get_llvm_zero(const Type* t)
  {
    return get_llvm_zero(generate_type(t));
  }

  llvm::Constant*
  Global_context::get_llvm_zero(llvm::Type* t) const
  {
    return llvm::Constant::getNullValue(t);
  }

  /// \todo We'll have to mangle the name based on the properties of the
  /// declaration. Note that only typed declarations have mangled names.
  std::string
  Global_context::generate_external_name(const Named_declaration* d)
  {
    return *d->get_name();
  }

  llvm::Type*
  Global_context::generate_type(const Typed_declaration* t)
  {
    return generate_type(t->get_type());
  }

  llvm::Type*
  Global_context::generate_type(const Type* t)
  {
    // Check to see if we've already generated the type.
    auto iter = m_types.find(t);
    if (iter != m_types.end())
      return iter->second;
    
    // Translate the type and memoize the result.
    llvm::Type* result = translate_type(t);
    m_types.emplace(t, result);
    return result;
  }

  llvm::Type*
  Global_context::translate_type(const Type* t)
  {
    switch (t->get_kind()) {
    default:
      __builtin_unreachable();
    case Type::bool_kind:
      return generate_bool_type(static_cast<const Bool_type*>(t));
    case Type::int_kind:
      return generate_int_type(static_cast<const Int_type*>(t));
    case Type::float_kind:
      return generate_float_type(static_cast<const Float_type*>(t));
    case Type::func_kind:
      return generate_function_type(static_cast<const Function_type*>(t));
    case Type::ref_kind:
      return generate_reference_type(static_cast<const Reference_type*>(t));
    }
  }

  llvm::Type*
  Global_context::generate_bool_type(const Bool_type* t)
  {
    return llvm::Type::getInt1Ty(*m_llvm);
  }

  llvm::Type*
  Global_context::generate_int_type(const Int_type* t)
  {
    // FIXME: Generate an appropriately ranked type.
    return llvm::Type::getInt32Ty(*m_llvm);
  }

  llvm::Type*
  Global_context::generate_float_type(const Float_type* t)
  {
    // FIXME: Generate an appropriately ranked type.    
    return llvm::Type::getFloatTy(*m_llvm);
  }

  /// \todo Handle variadic function types.
  llvm::Type*
  Global_context::generate_function_type(const Function_type* t)
  {
    const Type_seq& ps = t->get_parameter_types();
    std::vector<llvm::Type*> parms(ps.size());
    std::transform(ps.begin(), ps.end(), parms.begin(), [this](const Type* p) {
      return generate_type(p);
    });
    llvm::Type* ret = generate_type(t->get_return_type());
    llvm::Type* base = llvm::FunctionType::get(ret, parms, false);
    return base->getPointerTo();
  }

  llvm::Type*
  Global_context::generate_reference_type(const Reference_type* t)
  {
    llvm::Type* obj = generate_type(t->get_object_type());
    return obj->getPointerTo();
  }

  llvm::Constant*
  Global_context::generate_constant(const Type* t, const Value& v)
  {
    // FIXME: We can actually genenerte 
    switch (v.get_kind()) {
    default:
      break;
    case Value::int_kind:
      return get_llvm_int(t, v.get_int());
    case Value::float_kind:
      return get_llvm_float(t, v.get_float());
    }
    assert(false);
  }


  llvm::Constant*
  Global_context::generate_constant(const Object* o)
  {
    // FIXME: Generate aggregate constants for aggregate objects.
    return generate_constant(o->get_type(), o->load());
  }

} // namespace beaker
