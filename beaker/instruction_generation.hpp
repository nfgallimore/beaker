#pragma once

#include <beaker/common.hpp>

namespace llvm
{
  class LLVMContext;
  class Module;
  class Type;
  class Value;
  class Function;
  class BasicBlock;
} // namespace llvm

namespace beaker
{
  class Global_context;
  class Module_context;
  class Function_context;

  /// Provides context for translating a single statement or expression.
  /// Subexpressions and substatements are recursive generated by establishing
  /// a new context.
  class Instruction_generator
  {
  public:
    Instruction_generator(Function_context& parent);

    // Context

    /// Returns the Beaker context.
    Context& get_beaker_context() const;

    /// Returns the LLVM context.
    llvm::LLVMContext* get_llvm_context() const;

    /// Returns the LLVM module.
    llvm::Module* get_llvm_module() const;

    /// Returns the LLVM function.
    llvm::Function* get_llvm_function() const;

    /// Returns the global code generation context.
    Global_context& get_global_context();

    /// Returns the module code generation context.
    Module_context& get_module_context();

    /// Returns the function code generation context.
    Function_context& get_function_context() { return m_parent; }

    // Blocks

    /// Returns the current block.
    llvm::BasicBlock* get_current_block() const;

    /// Returns the entry block.
    llvm::BasicBlock* get_entry_block() const;

    /// Returns a new, unattached basic block.
    llvm::BasicBlock* make_block(const char* label);

    /// Emits `b`, making it the current block.
    void emit_block(llvm::BasicBlock* b);

    // Declarations

    /// Locally associate a declaration with its value.
    void declare(const Typed_declaration* d, llvm::Value* v);

    /// Returns the value associated with `d`.
    llvm::Value* lookup(const Typed_declaration* d);

    // Generation

    /// Generates the corresponding type for `t`.
    llvm::Type* generate_type(const Type* t);

    /// Generates the corresponding type for `d`.
    llvm::Type* generate_type(const Typed_declaration* d);

    /// Generate the sequence of instructions to compute `e`.
    llvm::Value* generate_expression(const Expression* e);
    
    // Core expressions
    llvm::Value* generate_bool_literal(const Bool_literal* e);
    llvm::Value* generate_int_literal(const Int_literal* e);
    llvm::Value* generate_id_expression(const Id_expression* e);

    // Arithmetic expressions
    llvm::Value* generate_addition_expression(const Addition_expression* e);
    llvm::Value* generate_subtraction_expression(const Subtraction_expression* e);
    llvm::Value* generate_negation_expression(const Negation_expression* e);
    llvm::Value* generate_multiplication_expression(const Multiplication_expression* e);
    llvm::Value* generate_quotient_expression(const Quotient_expression* e);
    llvm::Value* generate_remainder_expression(const Remainder_expression* e);
    llvm::Value* generate_division_expression(const Division_expression* e);
    llvm::Value* generate_reciprocal_expression(const Reciprocal_expression* e);
    
    // Relational expressions
    llvm::Value* generate_equal_to_expression(const Equal_to_expression* e);
    llvm::Value* generate_not_equal_to_expression(const Not_equal_to_expression* e);
    llvm::Value* generate_less_than_expression(const Less_than_expression* e);
    llvm::Value* generate_not_less_than_expression(const Not_less_than_expression* e);
    llvm::Value* generate_greater_than_expression(const Greater_than_expression* e);
    llvm::Value* generate_not_greater_than_expression(const Not_greater_than_expression* e);
    
    // Object expressions
    llvm::Value* generate_assignment_expression(const Assignment_expression* e);
    
    llvm::Value* generate_implicit_conversion(const Implicit_conversion* e);
    llvm::Value* generate_value_conversion(const Conversion* e);
    llvm::Value* generate_value_initialization(const Value_initializer* e);

    /// Generate the statement `s`.
    void generate_statement(const Statement* s);
    void generate_block_statement(const Block_statement* s);
    void generate_when_statement(const When_statement* s);
    void generate_if_statement(const If_statement* s);
    void generate_while_statement(const While_statement* s);
    void generate_break_statement(const Break_statement* s);
    void generate_continue_statement(const Continue_statement* s);
    void generate_return_statement(const Return_statement* s);
    void generate_expression_statement(const Expression_statement* s);
    void generate_declaration_statement(const Declaration_statement* s);

    /// Generate the local declaration `d`.
    void generate_declaration(const Declaration* d);
    void generate_value_declaration(const Value_declaration* d);
    void generate_variable_declaration(const Variable_declaration* d);
    void generate_reference_declaration(const Reference_declaration* d);
    void generate_assertion(const Assertion* d);

  private:
    /// The parent context.
    Function_context& m_parent;
  };

} // namespace beaker
