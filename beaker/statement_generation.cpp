#include "instruction_generation.hpp"
#include "module_generation.hpp"
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
      return generate_when_statement(static_cast<const When_statement*>(s));
    case Statement::if_kind:
      return generate_if_statement(static_cast<const If_statement*>(s));
    case Statement::while_kind:
      return generate_while_statement(static_cast<const While_statement*>(s));
    case Statement::break_kind:
      return generate_break_statement(static_cast<const Break_statement*>(s));
    case Statement::cont_kind:
      return generate_continue_statement(static_cast<const Continue_statement*>(s));
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
  Instruction_generator::generate_when_statement(const When_statement* s)
  {
    llvm::BasicBlock* true_block = make_block("when.true");
    llvm::BasicBlock* end_block = make_block("when.end");

    // Emit the condition and the branch.
    llvm::Value* v = generate_expression(s->get_condition());
    llvm::IRBuilder<> ir1(get_current_block());
    ir1.CreateCondBr(v, true_block, end_block);

    // Emit the true block and jump to end.
    emit_block(true_block);
    generate_statement(s->get_true_branch());
    llvm::IRBuilder<> ir2(get_current_block());
    ir2.CreateBr(end_block);

    // Make the end block active.
    emit_block(end_block);
  }

  void
  Instruction_generator::generate_if_statement(const If_statement* s)
  {
    llvm::BasicBlock* true_block = make_block("if.true");
    llvm::BasicBlock* false_block = make_block("if.false");
    llvm::BasicBlock* end_block = make_block("if.end");

    // Emit the condition and the branch.
    llvm::Value* v = generate_expression(s->get_condition());
    llvm::IRBuilder<> ir1(get_current_block());
    ir1.CreateCondBr(v, true_block, false_block);

    // Emit the true block and jump to end.
    emit_block(true_block);
    generate_statement(s->get_true_branch());
    llvm::IRBuilder<> ir2(get_current_block());
    ir2.CreateBr(end_block);

    // Emit the false block and jump to end.
    emit_block(false_block);
    generate_statement(s->get_false_branch());
    llvm::IRBuilder<> ir3(get_current_block());
    ir3.CreateBr(end_block);

    // Make the end block active.
    emit_block(end_block);
  }

  void
  Instruction_generator::generate_while_statement(const While_statement* s)
  {
    llvm::BasicBlock* if_block = make_block("while.if");
    llvm::BasicBlock* do_block = make_block("while.do");
    llvm::BasicBlock* end_block = make_block("while.end");

    // Emit a jump to the if block.
    llvm::IRBuilder<> ir1(get_current_block());
    ir1.CreateBr(if_block);

    // Emit the condition and conditional branch.
    emit_block(if_block);
    llvm::Value* cond = generate_expression(s->get_condition());
    llvm::IRBuilder<> ir2(get_current_block());
    ir2.CreateCondBr(cond, do_block, end_block);

    // Emit the loop body and jump to the top.
    emit_block(do_block);
    generate_statement(s->get_body());
    llvm::IRBuilder<> ir3(get_current_block());
    ir3.CreateBr(if_block);

    // Make the end block active.
    emit_block(end_block);
  }

  void
  Instruction_generator::generate_break_statement(const Break_statement* s)
  {

  }

  void
  Instruction_generator::generate_continue_statement(const Continue_statement* s)
  {

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
    case Declaration::assert_kind:
      return generate_assertion(static_cast<const Assertion*>(d));
    default:
      break;
    }
    assert(false);
  }

  /// Bind the declaration to it's computed initializer. Note that the
  /// initializer forces a load, which makes the value immutable, even when
  /// acquired from a variable.
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

  void
  Instruction_generator::generate_assertion(const Assertion* d)
  {
    llvm::Value* cond = generate_expression(d->get_condition());

    // FIXME: If the assertion is enabled, this should produce a call to a
    // builtin function that tests the condition. We shouldn't do it in
    // code.
    llvm::BasicBlock* ok = make_block("ok");
    llvm::BasicBlock* fail = make_block("fail");

    llvm::IRBuilder<> ir1(get_current_block());
    ir1.CreateCondBr(cond, ok, fail);

    // FIXME: Put intrinsics into the module.
    //
    // FIXME: Emit debugtrap only in debug mode. Or, however assertions end
    // up working at the end of the day.
    emit_block(fail);
    llvm::IRBuilder<> ir2(fail);
    llvm::Function* fn = get_module_context().get_debugtrap_intrinsic();
    ir2.CreateCall(fn, {});
    ir2.CreateUnreachable();

    // Make this block the current one.
    emit_block(ok);
  }

} // namespace beaker
