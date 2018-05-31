#pragma once

#include <beaker/common.hpp>

#include <queue>
#include <stack>
#include <unordered_map>

namespace llvm
{
  class LLVMContext;
  class Module;
  class Type;
  class Constant;
  class GlobalValue;
  class Function;
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
    using Global_map = std::unordered_map<const Typed_declaration*, llvm::GlobalValue*>;
  public:
    Module_context(Global_context& parent);

    // Context

    /// Returns the Beaker context.
    Context& get_beaker_context() const;

    /// Returns the LLVM context.
    llvm::LLVMContext* get_llvm_context() const;

    /// Returns the LLVM module.
    llvm::Module* get_llvm_module() const { return m_llvm; }

    /// Returns the global code generation context.
    Global_context& get_global_context() { return m_parent; }

    // Declarations

    /// Globally associate a declaration with its value.
    void declare(const Typed_declaration* d, llvm::GlobalValue* v);

    /// Returns the value associated with `d`.
    llvm::GlobalValue* lookup(const Typed_declaration* d);

    // Generation

    /// Generates the external name for `d`.
    std::string generate_external_name(const Named_declaration* d);

    /// Generates the type for `t`.
    llvm::Type* generate_type(const Type* t);

    /// Generates the type for `d`.
    llvm::Type* generate_type(const Typed_declaration* d);

    /// Generates a constant from a value.
    llvm::Constant* generate_constant(const Type* t, const Value& v);

    /// Generate a constant from an object.
    llvm::Constant* generate_constant(const Object* o);

    /// Recursively generate the contents of the translation unit.
    void generate_module(const Translation_unit* tu);

    /// Generates code corresponding for the declaration `d`.
    void generate_global(const Declaration* d);

    /// Generates a global variable.
    void generate_variable(const Data_declaration* d);

    /// Generates a function.
    void generate_function(const Function_declaration* d);

    // Functions

    /// Returns a function that acts as a constructor for the module. This
    /// will also append the function to the list of constructors for the
    /// module.
    llvm::Function* get_constructor(const char* name);

    // Constructors and destructors

    /// Called by variable initialization to generate a dynamic initialization
    /// function for given global variable.
    void add_constructor(const Variable_declaration* d);

    /// Generates a dynamic initialization function.
    llvm::Function* generate_constructor(const Variable_declaration* d, std::size_t);

    /// Generates the constructors for the module.
    void generate_constructors();

  private:
    /// The parent context.
    Global_context& m_parent;

    /// The Module being generated.
    llvm::Module* m_llvm;

    /// Global name bindings.
    Global_map m_globals;

    /// The list of global variables requiring global initialization.
    std::vector<const Variable_declaration*> m_ctors;
  };

} // namespace beaker
