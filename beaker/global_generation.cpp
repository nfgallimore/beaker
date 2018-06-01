#include "global_generation.hpp"
#include "type.hpp"
#include "declaration.hpp"
#include "value.hpp"
#include "object.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Constants.h>

namespace beaker
{
  namespace cg
  {
    // ------------------------------------------------------------------------ //
    // Factory

    llvm::Type*
    Factory::get_llvm_void_type()
    {
      return llvm::Type::getVoidTy(*m_cxt);
    }

    llvm::IntegerType*
    Factory::get_llvm_int_type(int n)
    {
      return llvm::Type::getIntNTy(*m_cxt, n);
    }

    llvm::IntegerType*
    Factory::get_llvm_byte_type()
    {
      // FIXME: This actually depends on the target architecture.
      return get_llvm_i8_type();
    }

    llvm::PointerType*
    Factory::get_llvm_pointer_type(llvm::Type* t)
    {
      return t->getPointerTo();
    }

    llvm::PointerType*
    Factory::get_llvm_void_pointer_type()
    {
      return get_llvm_pointer_type(get_llvm_void_type());
    }

    llvm::PointerType*
    Factory::get_llvm_void_function_pointer_type()
    {
      return get_llvm_pointer_type(get_llvm_void_function_type());
    }

    llvm::PointerType*
    Factory::get_llvm_byte_pointer_type()
    {
      return get_llvm_pointer_type(get_llvm_byte_type());
    }

    llvm::FunctionType*
    Factory::get_llvm_function_type(llvm::Type* r, llvm::ArrayRef<llvm::Type*> ts)
    {
      return llvm::FunctionType::get(r, ts, false);
    }

    llvm::FunctionType*
    Factory::get_llvm_nullary_function_type(llvm::Type* r)
    {
      return get_llvm_function_type(r, {});
    }

    llvm::FunctionType*
    Factory::get_llvm_void_function_type()
    {
      return get_llvm_nullary_function_type(get_llvm_void_type());
    }

    llvm::ArrayType*
    Factory::get_llvm_array_type(llvm::Type* t, std::size_t n)
    {
      return llvm::ArrayType::get(t, n);
    }

    llvm::StructType*
    Factory::get_llvm_struct_type(llvm::ArrayRef<llvm::Type*> a)
    {
      return llvm::StructType::create(a);
    }

    llvm::StructType*
    Factory::get_llvm_global_xtor_type()
    {
      if (!m_xtor_type) {
        llvm::Type* types[] {
          get_llvm_i32_type(), // i32
          get_llvm_void_function_pointer_type(), // void()*
          get_llvm_byte_pointer_type() // i8*
        };
        m_xtor_type = get_llvm_struct_type(types);
      }
      return m_xtor_type;
    }

    llvm::Constant*
    Factory::get_llvm_true()
    {
      return llvm::ConstantInt::getTrue(*m_cxt);
    }

    llvm::Constant*
    Factory::get_llvm_false()
    {
      return llvm::ConstantInt::getFalse(*m_cxt);
    }

    llvm::Constant*
    Factory::get_llvm_int(llvm::Type* t, std::intmax_t n)
    {
      // FIXME: Handle unsigned integer constants.
      return llvm::ConstantInt::getSigned(t, n);    
    }

    llvm::Constant*
    Factory::get_llvm_float(llvm::Type* t, double n)
    {
      return llvm::ConstantFP::get(t, n);
    }

    llvm::Constant*
    Factory::get_llvm_null_pointer(llvm::Type* t)
    {
      return llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(t)); 
    }

    llvm::Constant*
    Factory::get_llvm_null_void_pointer()
    {
      return llvm::ConstantPointerNull::get(get_llvm_void_pointer_type()); 
    }

    llvm::Constant*
    Factory::get_llvm_null_byte_pointer()
    {
      return llvm::ConstantPointerNull::get(get_llvm_byte_pointer_type()); 
    }

    llvm::Constant*
    Factory::get_llvm_zero(llvm::Type* t)
    {
      return llvm::Constant::getNullValue(t);
    }

    llvm::Constant*
    Factory::get_llvm_array(llvm::ArrayRef<llvm::Constant*> a)
    {
      assert(!a.empty());
      llvm::ArrayType* t = get_llvm_array_type(a[0]->getType(), a.size());
      return get_llvm_array(t, a);
    }

    llvm::Constant*
    Factory::get_llvm_array(llvm::ArrayType* t, llvm::ArrayRef<llvm::Constant*> a)
    {
      return llvm::ConstantArray::get(t, a);
    }

    llvm::Constant*
    Factory::get_llvm_struct(llvm::ArrayRef<llvm::Constant*> a)
    {
      return llvm::ConstantStruct::getAnon(a);
    }

    llvm::Constant*
    Factory::get_llvm_struct(llvm::StructType* t, llvm::ArrayRef<llvm::Constant*> a)
    {
      return llvm::ConstantStruct::get(t, a);
    }

  } // namespace cg

  // ------------------------------------------------------------------------ //
  // Global context

  Global_context_base::Global_context_base()
    : m_llvm(new llvm::LLVMContext())
  { }

  Global_context::Global_context(Context& cxt)
    : m_cxt(cxt), Global_context_base(), cg::Factory(m_llvm.get())
  { }

  Global_context::~Global_context()
  { }

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
      return get_llvm_int(generate_type(t), v.get_int());
    case Value::float_kind:
      return get_llvm_float(generate_type(t), v.get_float());
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
