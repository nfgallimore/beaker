#pragma once

#include <beaker/common.hpp>

namespace llvm
{
  class LLVMContext;
  class Module;
} // namespace llvm

namespace beaker
{
  class Translation_unit;
  class Value_declaration;
  class Variable_declaration;
  class Reference_declaration;
  class Function_declaration;
  class Global_context;

  /// Provides context for translating module-level constructs.
  class Module_context
  {
  public:
    Module_context(Global_context& parent, const Translation_unit* tu);

    // Context

    /// Returns the Beaker context.
    Context& get_beaker_context() const;

    /// Returns the LLVM context.
    llvm::LLVMContext* get_llvm_context() const;

    /// Recursively generate the contents of the module.
    void generate();

    /// Generates code corresponding for the declaration `d`.
    void generate(Declaration* d);

    /// Generates a global value declaration.
    void generate_variable(Value_declaration* d);

    /// Generates a global variable declaration.
    void generate_variable(Variable_declaration* d);

    /// Generates a global reference declaration.
    void generate_variable(Reference_declaration* d);

    /// Generates a function declaration.
    void generate_function(Function_declaration* d);
  
  private:
    /// The parent context.
    Global_context& m_parent;

    /// The current translation unit.
    const Translation_unit* m_tu;

    /// The Module being generated.
    llvm::Module* m_llvm;
  };

} // namespace beaker
