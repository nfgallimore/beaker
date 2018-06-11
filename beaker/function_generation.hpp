#pragma once

#include <beaker/common.hpp>

#include <unordered_map>

namespace llvm
{
  class LLVMContext;
  class Module;
  class Type;
  class FunctionType;
  class Value;
  class Function;
  class BasicBlock;
} // namespace llvm

namespace beaker
{
  class Function_declaration;
  class Global_context;
  class Module_context;

  /// Provides context for translating function definitions.
  class Function_context
  {
    using Value_map = std::unordered_map<const Typed_declaration*, llvm::Value*>;
  public:
    Function_context(Module_context& parent);
    Function_context(Module_context& parent, llvm::Function* fn);

    // Context

    /// Returns the Beaker context.
    Context& get_beaker_context() const;

    /// Returns the LLVM context.
    llvm::LLVMContext* get_llvm_context() const;

    /// Returns the LLVM module.
    llvm::Module* get_llvm_module() const;

    /// Returns the global code generation context.
    Global_context& get_global_context();

    /// Returns the module code generation context.
    Module_context& get_module_context() { return m_parent; }

    // Blocks

    /// Returns the current block.
    llvm::BasicBlock* get_current_block() const { return m_block; }

    /// Returns the entry block for the function.
    llvm::BasicBlock* get_entry_block() const { return m_entry; }

    /// Returns a new, unattached basic block.
    llvm::BasicBlock* make_block(const char* label);

    /// Emits the `b`, making it the current block.
    void emit_block(llvm::BasicBlock* b);

    // Declarations

    /// Locally associate a declaration with its value.
    void declare(const Typed_declaration* d, llvm::Value* v);

    /// Returns the value associated with `d`.
    llvm::Value* lookup(const Typed_declaration* d);

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

    /// Generate declarations for the parameters of the function.
    void generate_parameters(const Function_declaration* d);

    /// Generate a function definition comprised of a compound statement.
    void generate_definition(const Statement* s);

    /// Generate a function definition comprised of a single expression.
    /// This is used to generate internally used functions or thunks whose
    /// definitions are trivial (e.g., dynamic initializers).
    void generate_definition(const Expression* e);

    // Definition

    // Used to establish the entry block and initialize arguments. 
    //
    // \note This can be called externally when defining a function whose 
    // definition is not generated.
    void start_definition();

    // Used to perform final cleanups of the function definition.
    //
    // \note This can be called externally when defining a function whose 
    // definition is not generated.
    void finish_definition();

  private:
    /// The parent context.
    Module_context& m_parent;

    /// The function being generated.
    llvm::Function* m_llvm;

    /// The current block.
    llvm::BasicBlock* m_block;

    /// The entry block.
    llvm::BasicBlock* m_entry;

    /// Local name bindings.
    Value_map m_locals;
  };

} // namespace beaker
