#pragma once

#include <beaker/common.hpp>

namespace llvm
{
  class LLVMContext;
  class Module;
  class Type;
  class Value;
  class Constant;
  class GlobalVariable;
} // namespace llvm

namespace beaker
{
  class Function_declaration;
  class Global_context;
  class Module_context;

  /// Provides context for translating global variables.
  class Variable_context
  {
  public:
    Variable_context(Module_context& parent);

    // Context

    /// Returns the Beaker context.
    Context& get_beaker_context() const;

    /// Returns the LLVM context.
    llvm::LLVMContext* get_llvm_context() const;

    /// Returns the LLVM module
    llvm::Module* get_llvm_module() const;

    /// Returns the global code generation context.
    Global_context& get_global_context();

    /// Returns the module code generation context.
    Module_context& get_module_context() { return m_parent; }

    // Generation facilities

    /// Generates the external name for `d`.
    std::string generate_external_name(const Named_declaration* d);

    /// Generates the corresponding type for `t`.
    llvm::Type* generate_type(const Type* t);

    /// Generates the corresponding type for `d`.
    llvm::Type* generate_type(const Typed_declaration* d);

    /// Generates a constant from a value.
    llvm::Constant* generate_constant(const Type* t, const Value& v);

    /// Generate a constant from an object.
    llvm::Constant* generate_constant(const Object* o);

    /// Generate the global variable `d`.
    void generate(const Data_declaration* d);

    llvm::Constant* generate_static_initializer(const Data_declaration* d);
    llvm::Constant* generate_constant_initializer(const Data_declaration* d);
    llvm::Constant* generate_zero_initializer(const Data_declaration* d);

    void generate_dynamic_initializer(const Data_declaration* d);

  private:
    /// The parent context.
    Module_context& m_parent;

    /// The variable being generated.
    llvm::GlobalVariable* m_llvm;
  };

} // namespace beaker
