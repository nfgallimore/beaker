#pragma once

#include <beaker/common.hpp>
#include <beaker/token.hpp>

namespace beaker
{
  class Declaration_context;
  class Named_declaration;
  class Typed_declaration;

  /// The semantics class implements the semantic actions of the parser.
  class Semantics
  {
  public:
    Semantics(Context& cxt);
    ~Semantics();

    Type_specifier* on_unit_type(const Token& tok);
    Type_specifier* on_bool_type(const Token& tok);
    Type_specifier* on_int_type(const Token& tok);
    Type_specifier* on_float_type(const Token& tok);
    Type_specifier* on_char_type(const Token& tok);

    Expression* on_assignment_expression(Expression* e1,
                                         Expression* e2,
                                         const Token& op);
    
    Expression* on_conditional_expression(Expression* e1, 
                                          Expression* e2, 
                                          Expression* e3,
                                          const Token& question,
                                          const Token& colon);
    
    Expression* on_logical_expression(Expression* e1, 
                                      Expression* e2,
                                      const Token& op);
    
    Expression* on_bitwise_expression(Expression* e1, 
                                      Expression* e2,
                                      const Token& op);
    
    Expression* on_equality_expression(Expression* e1, 
                                       Expression* e2,
                                       const Token& op);
    
    Expression* on_relational_expression(Expression* e1, 
                                         Expression* e2,
                                         const Token& op);
    
    Expression* on_shift_expression(Expression* e1, 
                                    Expression* e2,
                                    const Token& op);
    
    Expression* on_additive_expression(Expression* e1, 
                                       Expression* e2,
                                       const Token& op);
    
    Expression* on_multiplicative_expression(Expression* e1, 
                                             Expression* e2,
                                             const Token& op);
    
    Expression* on_cast_expression(Expression* e, 
                                   Type_specifier* t);
    
    Expression* on_unary_expression(Expression* e, 
                                    const Token& op);
    
    Expression* on_call_expression(Expression* e, 
                                   const Expression_seq& args,
                                   const Token& lparen,
                                   const Token& rparen);
    
    Expression* on_subscript_expression(Expression* e, 
                                        const Expression_seq& args,
                                        const Token& lparen,
                                        const Token& rparen);
    
    Expression* on_unit_literal(const Token& tok);
    Expression* on_integer_literal(const Token& tok);
    Expression* on_boolean_literal(const Token& tok);
    Expression* on_float_literal(const Token& tok);
    Expression* on_character_literal(const Token& tok);
    Expression* on_string_literal(const Token& tok);

    Expression* on_id_expression(const Token& id);

    Expression* on_paren_expression(Expression* e,
                                    const Token& lparen, 
                                    const Token& rparen);

    Statement* on_block_statement(const Statement_seq& stmts,
                                  const Token& lbrace,
                                  const Token& rbrace);

    Statement* on_when_statement(Expression* e, 
                                 Statement* s, 
                                 const Token& kw,
                                 const Token& lparen,
                                 const Token& rparen);
    
    Statement* on_if_statement(Expression* e, 
                               Statement* s1, 
                               Statement* s2,
                               const Token& kw1,
                               const Token& lparen,
                               const Token& rparen,
                               const Token& kw2);
    
    Statement* on_while_statement(Expression* e, 
                                  Statement* s,
                                  const Token& kw,
                                  const Token& lparen,
                                  const Token& rparen);
    
    Statement* on_break_statement(const Token& kw, 
                                  const Token& semi);
    
    Statement* on_continue_statement(const Token& kw,
                                     const Token& semi);

    Statement* on_return_statement(Expression* e,
                                   const Token& kw,
                                   const Token& semi);
    
    Statement* on_declaration_statement(Declaration* d);
    
    Statement* on_expression_statement(Expression* e, 
                                       const Token& semi);

    // Conditions
    Condition* on_condition(Expression* e);

    // Declarations

    /// Called to note the beginning of a translation unit.
    Declaration* on_start_translation();
    
    /// Called to note the end of the translation unit.
    Declaration* on_finish_translation(Declaration*);

    /// Called to note the identification of a data entity.
    Declaration* on_data_identification(const Token& id, const Token& kw);

    /// Called to note the declaration of an untyped data entity.
    Declaration* on_data_declaration(Declaration* d);

    /// Called to note the declaration of a typed data entity.
    Declaration* on_data_declaration(Declaration* d, Type_specifier* t);

    /// Called to note the default initialization of a data entity.
    Declaration* on_data_initialization(Declaration* d,
                                        const Token& semi);

    /// Called to note the initialization of a data entity by an expression.
    Declaration* on_data_initialization(Declaration* d, 
                                        Expression* e,
                                        const Token& semi);

    /// Called to note the identification of a function.
    Declaration* on_function_identification(const Token& id, const Token& kw);

    /// Called to note the declaration of a function.
    Declaration* on_function_declaration(Declaration* d, 
                                         Parameter_seq&& parms, 
                                         Type_specifier* type,
                                         const Token& lparen,
                                         const Token& rparen,
                                         const Token& arrow);
    
    Declaration* on_function_definition(Declaration* d, 
                                        Statement_seq&& ss,
                                        const Token& lbrace,
                                        const Token& rbrace);

    Parameter* on_function_parameter(const Token& n, 
                                     Type_specifier* t, 
                                     const Token& colon);

    Parameter* on_variadic_parameter(const Token& ellipsis);
    Parameter* on_variadic_parameter(const Token& ellipsis, const Token& id);

    // Declaration context

  private:
    void enter_scope(Scoped_declaration* sd);
    void leave_scope(Scoped_declaration* sd);

  public:
    /// Make `d` the active declarative region.
    void enter_scope(Declaration* d);

    /// Make `s` the active declarative region.
    void enter_scope(Statement* s);

    /// Restore the previous declarative region. 
    void leave_scope(Declaration* d);

    /// Restore the previous declarative region. 
    void leave_scope(Statement* s);

    /// Returns current scoped declaration. This provides lookup and
    /// declaration facilities for the current declarative region.
    Scoped_declaration* get_current_declaration() const { return m_sd; }

    // Declarations

    void identify(Named_declaration* d);
    void declare(Typed_declaration* d);
    void declare(Named_declaration* d);

    // Lookup

    Declaration* unqualified_lookup(Symbol s);

  private:
    /// The translation context. This provides access to compiler resources.
    Context& m_cxt;

    /// The current declaration (i.e., scope).
    Scoped_declaration* m_sd;
  };

} // namespace beaker
