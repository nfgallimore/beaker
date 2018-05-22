#pragma once

#include <beaker/common.hpp>

namespace llvm
{
  class LLVMContext;
  class Function;
} // namespace llvm

namespace beaker
{
  class Function_declaration;
  class Module_context;

  /// Provides context for translating function definitions.
  class Function_context
  {
  public:
    Function_context(Module_context& parent, const Function_declaration* fn);

    // Context

    /// Returns the Beaker context.
    Context& get_beaker_context() const;

    /// Returns the LLVM context.
    llvm::LLVMContext* get_llvm_context() const;

    /// Recursively generate the definition of the function.
    void generate();

  private:
    /// The parent context.
    Module_context& m_parent;

    /// The current translation unit.
    const Function_declaration* m_fn;

    /// The function being generated.
    llvm::Function* m_llvm;
  };

} // namespace beaker
