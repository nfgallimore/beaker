#pragma once

#include <beaker/common.hpp>

#include <unordered_map>

namespace llvm
{
  class LLVMContext;
  class Type;
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

    // Names

    /// Returns a name for the declaration that is suitable for linking
    /// between modules.
    std::string generate_external_name(const Named_declaration* d);

    // Types

    /// Generate the corresponding type for `t`.
    llvm::Type* generate_type(const Type* t);

    /// Generate the corresponding type for `t`. This does not check for
    /// a cached representation.
    llvm::Type* generate_type_impl(const Type* t);

    /// Returns `i1`.
    llvm::Type* generate_bool_type(const Bool_type* t);

    /// Returns the corresponding integer type.
    llvm::Type* generate_int_type(const Int_type* t);

    /// Returns the corresponding floating point type.
    llvm::Type* generate_float_type(const Float_type* t);

    /// Returns a pointer to the corresponding function type.
    llvm::Type* generate_function_type(const Function_type* t);
    
    /// Retuns a pointer for the reference type.
    llvm::Type* generate_reference_type(const Reference_type* t);

    /// Returns the corresponding type for the declaration `d`.
    llvm::Type* generate_type(const Typed_declaration* d);

  private:
    /// The Beaker translation context.
    Context& m_cxt;

    /// The LLVM context.
    std::unique_ptr<llvm::LLVMContext> m_llvm;

    /// Stores previously translated types.
    Type_map m_types;
  };

} // namespace beaker
