#pragma once

#include <beaker/common.hpp>

namespace llvm
{
  class LLVMContext;
  class Module;
  class Type;
  class FunctionType;
  class Function;
  class BasicBlock;
} // namespace llvm

namespace beaker
{
  class Function_declaration;
  class Module_context;

  /// Provides context for translating function definitions.
  class Function_context
  {
  public:
    Function_context(Module_context& parent);

    // Context

    /// Returns the Beaker context.
    Context& get_beaker_context() const;

    /// Returns the LLVM context.
    llvm::LLVMContext* get_llvm_context() const;

    /// Returns the LLVM module.
    llvm::Module* get_llvm_module() const;

    // Blocks

    /// Returns the current block.
    llvm::BasicBlock* get_current_block() const { return m_block; }

    /// Emits the `b`, making it the current block.
    void emit_block(llvm::BasicBlock* b);

    // Generation

    /// Returns the mangled external name for `d`.
    std::string generate_external_name(const Named_declaration* d);

    /// Returns the type corresponding to `t`.
    llvm::Type* generate_type(const Type* t);

    /// Returns the type for the declaration `d`.
    llvm::Type* generate_type(const Typed_declaration* d);

    /// Returns the function type for `d`. Note that function types typically
    /// correspond to function pointers; this yields an actual function type.
    llvm::FunctionType* generate_function_type(const Function_declaration* d);

    /// Recursively generate the definition of the function.
    void generate(const Function_declaration* d);

  private:
    /// The parent context.
    Module_context& m_parent;

    /// The function being generated.
    llvm::Function* m_llvm;

    /// The current block.
    llvm::BasicBlock* m_block;
  };

} // namespace beaker
