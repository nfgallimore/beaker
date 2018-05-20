#include "semantics.hpp"
#include "type.hpp"
#include "expression.hpp"
#include "statement.hpp"
#include "declaration.hpp"
#include "context.hpp"
#include "print.hpp"

#include <iostream>
#include <sstream>

namespace beaker
{
  Statement*
  Semantics::on_start_block_statement()
  {
    Block_statement* bs = new Block_statement();

    // Add this statement to the current block.
    Block_statement* block = get_current_block();
    block->add_statement(bs);

    return bs;
  }

  Statement*
  Semantics::on_finish_block_statement(Statement* s,
                                       const Token& lbrace,
                                       const Token& rbrace)
  {
    Block_statement* bs = static_cast<Block_statement*>(s);

    // Update the statement.
    bs->set_brace_locations(lbrace.get_location(), rbrace.get_location());

    return bs;
  }

  Statement*
  Semantics::on_when_statement(Expression* e, 
                               Statement* s, 
                               const Token& kw,
                               const Token& lparen,
                               const Token& rparen)
  {
    return nullptr;
  }

  Statement*
  Semantics::on_if_statement(Expression* e, 
                             Statement* s1, 
                             Statement* s2,
                             const Token& kw1,
                             const Token& lparen,
                             const Token& rparen,
                             const Token& kw2)
  {
    return nullptr;
  }

  Statement*
  Semantics::on_while_statement(Expression* e, 
                                Statement* s,
                                const Token& kw,
                                const Token& lparen,
                                const Token& rparen)
  {
    return nullptr;
  }

  Statement*
  Semantics::on_break_statement(const Token& kw, 
                                const Token& semi)
  {
    return nullptr;
  }

  Statement*
  Semantics::on_continue_statement(const Token& kw,
                                   const Token& semi)
  {
    return nullptr;
  }

  Statement*
  Semantics::on_return_statement(Expression* e,
                                 const Token& kw,
                                 const Token& semi)
  {
    // Convert the return value to the function return type.
    Type* ret = get_current_function()->get_return_type();
    e = convert_to_type(e, ret);

    // Create the statement.
    Location start = kw.get_location();
    Location end = semi.get_location();
    Return_statement* es = new Return_statement(e, start, end);

    Block_statement* block = get_current_block();
    block->add_statement(es);

    return es;
  }

  Statement*
  Semantics::on_declaration_statement(Declaration* d)
  {
    return nullptr;
  }

  Statement*
  Semantics::on_expression_statement(Expression* e, 
                                     const Token& semi)
  {
    // Create and register the statement.
    //
    // FIXME: Is there any analysis we need to do?
    Location end = semi.get_location();
    Expression_statement* es = new Expression_statement(e, end);
    
    Block_statement* block = get_current_block();
    block->add_statement(es);

    return es;
  }

  // Conditions

  Condition* 
  Semantics::on_condition(Expression* e)
  {
    return nullptr;
  }

} // namespace beaker
