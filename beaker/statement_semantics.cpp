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
    return new Block_statement();
  }

  Statement*
  Semantics::on_finish_block_statement(Statement* s,
                                       Statement_seq&& ss,
                                       const Token& lbrace,
                                       const Token& rbrace)
  {
    Block_statement* bs = static_cast<Block_statement*>(s);

    // Update the statement.
    bs->set_statements(std::move(ss));
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
    /// The condition is converted to bool.
    ///
    /// FIXME: Process the expression as a legit condition.
    e = convert_to_bool(e);

    Location start = kw.get_location();
    Location lloc = lparen.get_location();
    Location rloc = rparen.get_location();
    return new When_statement(e, s, start, lloc, rloc);
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
    /// The condition is converted to bool.
    ///
    /// FIXME: Process the expression as a legit condition.
    e = convert_to_bool(e);

    Location start = kw1.get_location();
    Location lloc = lparen.get_location();
    Location rloc = rparen.get_location();
    Location eloc = kw2.get_location();
    return new If_statement(e, s1, s2, start, lloc, rloc, eloc);
  }

  Statement*
  Semantics::on_while_statement(Expression* e, 
                                Statement* s,
                                const Token& kw,
                                const Token& lparen,
                                const Token& rparen)
  {
    /// The condition is converted to bool.
    ///
    /// FIXME: Process the expression as a legit condition.
    e = convert_to_bool(e);

    Location start = kw.get_location();
    Location lloc = lparen.get_location();
    Location rloc = rparen.get_location();
    return new While_statement(e, s, start, lloc, rloc);
  }

  Statement*
  Semantics::on_break_statement(const Token& kw, 
                                const Token& semi)
  {
    // FIXME: Verify that break is valid in this context.
    Location start = kw.get_location();
    Location end = semi.get_location();
    return new Break_statement(start, end);
  }

  Statement*
  Semantics::on_continue_statement(const Token& kw,
                                   const Token& semi)
  {
    Location start = kw.get_location();
    Location end = semi.get_location();
    return new Continue_statement(start, end);
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
    return new Return_statement(e, start, end);
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
    return new Expression_statement(e, end);
  }

  // Conditions

  Condition* 
  Semantics::on_condition(Expression* e)
  {
    return nullptr;
  }

} // namespace beaker
