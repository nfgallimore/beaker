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
    case Statement::decl_kind:
    default:
      break;
    }
    assert(false);
  }

  void
  Instruction_generator::generate_block_statement(const Block_statement* s)
  {
    for (Statement* sub : s->get_statements())
      return generate_statement(sub);
  }

  void
  Instruction_generator::generate_return_statement(const Return_statement* s)
  {
    llvm::Value* v = generate_expression(s->get_return_value());
    llvm::IRBuilder<> ir(get_current_block());
    ir.CreateRet(v);
  }

} // namespace beaker
