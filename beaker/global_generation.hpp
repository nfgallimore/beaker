#pragma once

#include <beaker/common.hpp>

#include <unordered_map>

namespace llvm
{
  template<typename T> class ArrayRef;

  class LLVMContext;
  class Module;
  
  class Type;
  class IntegerType;
  class PointerType;
  class FunctionType;
  class ArrayType;
  class StructType;
  
  class Value;
  class GlobalValue;
  class Constant;
  class GlobalVariable;
  class Function;
} // namespace llvm

namespace beaker
{
  class Bool_type;
  class Int_type;
  class Float_type;
  class Function_type;
  class Reference_type;


  /// Stores a mapping of known Beaker types to LLVM types.
  using Type_map = std::unordered_map<const Type*, llvm::Type*>;

  namespace cg
  {
    class Factory
    {
    protected:
      Factory(llvm::LLVMContext* cxt)
        : m_cxt(cxt), m_xtor_type()
      { }

    public:
      // LLVM types

      /// Returns the LLVM void type.
      llvm::Type* get_llvm_void_type();
      
      /// Returns an LLVM int type with `n` bits of precision.
      llvm::IntegerType* get_llvm_int_type(int n);
      
      /// Returns an LLVM int type with 1 bit of precision.
      llvm::IntegerType* get_llvm_i1_type() { return get_llvm_int_type(1); }

      /// Returns an LLVM int type with 8 bits of precision.
      llvm::IntegerType* get_llvm_i8_type() { return get_llvm_int_type(8); }

      /// Returns an LLVM int type with 16 bits of precision.
      llvm::IntegerType* get_llvm_i16_type() { return get_llvm_int_type(16); }
      
      /// Returns an LLVM int type with 32 bits of precision.
      llvm::IntegerType* get_llvm_i32_type() { return get_llvm_int_type(32); }

      /// Returns an LLVM int type with 64 bits of precision.
      llvm::IntegerType* get_llvm_i64_type() { return get_llvm_int_type(64); }

      /// Returns the LLVM type commonly used to represent bytes.
      llvm::IntegerType* get_llvm_byte_type();

      /// Returns the pointer type `t*`.
      llvm::PointerType* get_llvm_pointer_type(llvm::Type* t);

      /// Returns the pointer type `void*`.
      llvm::PointerType* get_llvm_void_pointer_type();

      /// Returns the pointer type `void()*`.
      llvm::PointerType* get_llvm_void_function_pointer_type();

      /// Returns the type pointer-to-byte.
      llvm::PointerType* get_llvm_byte_pointer_type();

      /// Returns the function type `r(ts...)`.
      llvm::FunctionType* get_llvm_function_type(llvm::Type* r, llvm::ArrayRef<llvm::Type*> ts);

      /// Returns the function type `r()`.
      llvm::FunctionType* get_llvm_nullary_function_type(llvm::Type* r);

      /// Returns the function type `void()`.
      llvm::FunctionType* get_llvm_void_function_type();

      /// Returns an array type `[t x n]`.
      llvm::ArrayType* get_llvm_array_type(llvm::Type* t, std::size_t n);

      /// Returns a struct type whose members are in `a`.
      llvm::StructType* get_llvm_struct_type(llvm::ArrayRef<llvm::Type*> a);

      /// Returns the type of a global constructor or destructor.
      llvm::StructType* get_llvm_global_xtor_type();

      // LLVM constants

      /// Returns the constant true.
      llvm::Constant* get_llvm_true();

      /// Returns the constant false.
      llvm::Constant* get_llvm_false();

      /// Returns the integer value n.
      llvm::Constant* get_llvm_int(llvm::Type* t, std::intmax_t n);

      /// Returns the floating point value n.
      llvm::Constant* get_llvm_float(llvm::Type* t, double n);

      /// Returns the null pointer for a pointer type.
      llvm::Constant* get_llvm_null_pointer(llvm::Type* t);

      /// Returns the null void pointer.
      llvm::Constant* get_llvm_null_void_pointer();

      /// Returns the null byte pointer.
      llvm::Constant* get_llvm_null_byte_pointer();

      /// Returns the zero initializer for the given type.
      llvm::Constant* get_llvm_zero(llvm::Type* t);

      /// Returns an array of the elements in `a`. The types of all elements
      /// must be the same.
      llvm::Constant* get_llvm_array(llvm::ArrayRef<llvm::Constant*> a);
      
      /// Returns an array of the elements in `a`. The types of all elements
      /// must be `t`.
      llvm::Constant* get_llvm_array(llvm::ArrayType* t, llvm::ArrayRef<llvm::Constant*> a);

      /// Returns an anonymous LLVM structure constant for the constant `a`.
      llvm::Constant* get_llvm_struct(llvm::ArrayRef<llvm::Constant*> a);

      /// Returns an LLVM structure constant for the constant in `a`.
      llvm::Constant* get_llvm_struct(llvm::StructType* t, llvm::ArrayRef<llvm::Constant*> a);

    private:
      llvm::LLVMContext* m_cxt;

      /// The cached xtor type.
      llvm::StructType* m_xtor_type;
    };
  } // namespace cg



  /// A helper class that is used to ensure the initialization of the LLVM
  /// context before the the underlying factory.
  class Global_context_base
  {
  protected:
    Global_context_base();

  protected:
    /// The LLVM context.
    std::unique_ptr<llvm::LLVMContext> m_llvm;
  };


  /// Provides context for translating global constructs (i.e., types and
  /// constants).
  ///
  /// \todo Add support for generating constants.
  class Global_context : public Global_context_base, public cg::Factory
  {
  public:
    Global_context(Context& cxt);
    ~Global_context();

    // Context

    /// Returns the Beaker context.
    Context& get_beaker_context() const { return m_cxt; }

    /// Returns the LLVM context.
    llvm::LLVMContext* get_llvm_context() const { return m_llvm.get(); }

    // FIXME: Implement this...

    // Names

    /// Returns a name for the declaration that is suitable for linking
    /// between modules.
    std::string generate_external_name(const Named_declaration* d);

    // Types

    /// Generate the corresponding type for `t`.
    llvm::Type* generate_type(const Type* t);
    llvm::Type* generate_bool_type(const Bool_type* t);
    llvm::Type* generate_int_type(const Int_type* t);
    llvm::Type* generate_float_type(const Float_type* t);
    llvm::Type* generate_function_type(const Function_type* t);    
    llvm::Type* generate_reference_type(const Reference_type* t);

    /// Generate the corresponding type for `t`. This does not check for
    /// a cached representation.
    llvm::Type* translate_type(const Type* t);
    

    llvm::Type* generate_type(const Typed_declaration* d);

    // Constants

    /// Generates a constant from a value.
    llvm::Constant* generate_constant(const Type* t, const Value& v);

    /// Generate a constant from an object.
    llvm::Constant* generate_constant(const Object* o);

  private:
    /// The Beaker translation context.
    Context& m_cxt;

    /// Stores previously translated types.
    Type_map m_types;
  };

} // namespace beaker
