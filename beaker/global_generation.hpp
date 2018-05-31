#pragma once

#include <beaker/common.hpp>

#include <unordered_map>

namespace llvm
{
  class LLVMContext;
  class Type;
  class Constant;
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


  /// Provides context for translating global constructs (i.e., types and
  /// constants).
  ///
  /// \todo Add support for generating constants.
  class Global_context
  {
  public:
    Global_context(Context& cxt);
    ~Global_context();

    // Context

    /// Returns the Beaker context.
    Context& get_beaker_context() const { return m_cxt; }

    /// Returns the LLVM context.
    llvm::LLVMContext* get_llvm_context() const { return m_llvm.get(); }

    // LLVM types

    /// Returns the LLVM void type.
    llvm::Type* get_llvm_void_type() const;
    
    /// Returns an LLVM int type with `n` bits of precision.
    llvm::Type* get_llvm_int_type(int n) const;
    
    /// Returns an LLVM int type with 1 bit of precision.
    llvm::Type* get_llvm_i1_type() const { return get_llvm_int_type(1); }

    /// Returns an LLVM int type with 8 bits of precision.
    llvm::Type* get_llvm_i8_type() const { return get_llvm_int_type(8); }

    /// Returns an LLVM int type with 16 bits of precision.
    llvm::Type* get_llvm_i16_type() const { return get_llvm_int_type(16); }
    
    /// Returns an LLVM int type with 32 bits of precision.
    llvm::Type* get_llvm_i32_type() const { return get_llvm_int_type(32); }

    /// Returns an LLVM int type with 64 bits of precision.
    llvm::Type* get_llvm_i64_type() const { return get_llvm_int_type(64); }

    // LLVM constants

    /// Returns the constant true.
    llvm::Constant* get_llvm_true();

    /// Returns the constant false.
    llvm::Constant* get_llvm_false();

    /// Returns the integer value n.
    llvm::Constant* get_llvm_int(const Type* t, std::intmax_t n);
    
    /// Returns the integer value n.
    llvm::Constant* get_llvm_int(llvm::Type* t, std::intmax_t n) const;

    /// Returns a floating point value.
    llvm::Constant* get_llvm_float(const Type* t, double n);

    /// Returns the null pointer for a pointer type.
    llvm::Constant* get_llvm_null(llvm::Type* t) const;

    /// Returns the zero initializer for the given type.
    llvm::Constant* get_llvm_zero(const Type* t);
    
    /// Returns the zero initializer for the given type.
    llvm::Constant* get_llvm_zero(llvm::Type* t) const;

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

    /// The LLVM context.
    std::unique_ptr<llvm::LLVMContext> m_llvm;

    /// Stores previously translated types.
    Type_map m_types;
  };

} // namespace beaker
