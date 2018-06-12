#include "instruction_generation.hpp"
#include "expression.hpp"
#include "statement.hpp"
#include "declaration.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>

namespace beaker
{
  void
  Instruction_generator::generate_statement(const Statement* s)
  {
    switch (s->get_kind()) {
    case Statement::block_kind:
      return generate_block_statement(static_cast<const Block_statement*>(s));
    case Statement::when_kind:
    case Statement::if_kind:
    case Statement::while_kind:
    case Statement::break_kind:
    case Statement::cont_kind:
      break;
    case Statement::ret_kind:
      return generate_return_statement(static_cast<const Return_statement*>(s));
    case Statement::expr_kind:
      return generate_expression_statement(static_cast<const Expression_statement*>(s));
    case Statement::decl_kind:
      return generate_declaration_statement(static_cast<const Declaration_statement*>(s));
    default:
      break;
    }
    assert(false);
  }

  void
  Instruction_generator::generate_block_statement(const Block_statement* s)
  {
    for (Statement* sub : s->get_statements())
      generate_statement(sub);
  }

  void
  Instruction_generator::generate_return_statement(const Return_statement* s)
  {
    llvm::Value* v = generate_expression(s->get_return_value());
    llvm::IRBuilder<> ir(get_current_block());
    ir.CreateRet(v);
  }

  void
  Instruction_generator::generate_expression_statement(const Expression_statement* s)
  {
    generate_expression(s->get_expression());
  }

  void
  Instruction_generator::generate_declaration_statement(const Declaration_statement* s)
  {
    generate_declaration(s->get_declaration());
  }

  // Local declarations

  void
  Instruction_generator::generate_declaration(const Declaration* d)
  {
    switch (d->get_kind()) {
    case Declaration::val_kind:
      return generate_value_declaration(static_cast<const Value_declaration*>(d));
    case Declaration::var_kind:
      return generate_variable_declaration(static_cast<const Variable_declaration*>(d));
    case Declaration::ref_kind:
      return generate_reference_declaration(static_cast<const Reference_declaration*>(d));
    default:
      break;
    }
    assert(false);
  }

  /// Bind the declaration to it's computed initializer.
  void
  Instruction_generator::generate_value_declaration(const Value_declaration* d)
  {
    llvm::Value* val = generate_expression(d->get_initializer());
    declare(d, val);
  }

  /// Allocate storage for the variable and apply the initializer.
  void
  Instruction_generator::generate_variable_declaration(const Variable_declaration* d)
  {
    // Create the storage for the variable in the entry block.
    llvm::IRBuilder<> ir(get_entry_block());
    llvm::Type* type = generate_type(d);
    llvm::Value* addr = ir.CreateAlloca(type);
    declare(d, addr);

    // Emit the initializer in the current block. Note that initializers are
    // effectively void.
    generate_expression(d->get_initializer());
  }

  /// Bind the declaration to it's computed initializer.
  void
  Instruction_generator::generate_reference_declaration(const Reference_declaration* d)
  {
    llvm::Value* val = generate_expression(d->get_initializer());
    declare(d, val);
  }


} // namespace beaker
