#include "semantics.hpp"
#include "statement.hpp"
#include "context.hpp"
#include "print.hpp"

#include <iostream>
#include <sstream>

namespace beaker
{
  Statement*
  Semantics::on_block_statement(const Statement_seq& stmts,
                                const Token& lbrace,
                                const Token& rbrace)
  {
    return nullptr;
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
    return nullptr;
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
    return nullptr;
  }

  // Conditions

  Condition* 
  Semantics::on_condition(Expression* e)
  {
    return nullptr;
  }

} // namespace beaker
