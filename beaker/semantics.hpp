#pragma once

#include <beaker/common.hpp>
#include <beaker/token.hpp>
#include <beaker/dump.hpp>

namespace beaker
{
  class Scope;
  class Declaration_set;
  class Int_type;
  class Float_type;
  class Function_type;
  class Reference_type;
  class Block_statement;

  /// The semantics class implements the semantic actions of the parser.
  class Semantics
  {
  public:
    Semantics(Context& cxt);
    ~Semantics();

    /// Returns the underlying context.
    Context& get_context() const { return m_cxt; }

    /// Invoked to construct a reference type.
    Type_specifier* on_reference_type(Type_specifier* ts, const Token& tok);

    /// Invoked to construct a unit type.
    Type_specifier* on_unit_type(const Token& tok);

    /// Invoked to construct a unit type.
    Type_specifier* on_bool_type(const Token& tok);

    /// Invoked to construct a unit type.
    Type_specifier* on_int_type(const Token& tok);

    /// Invoked to construct a unit type.
    Type_specifier* on_float_type(const Token& tok);

    /// Invoked to construct a unit type.
    Type_specifier* on_char_type(const Token& tok);

    /// Invoked to construct a function type.
    Type_specifier* on_function_type(Type_specifier_seq&& parms,
                                     Type_specifier* ret, 
                                     const Token& lparen,
                                     const Token& rparen,
                                     const Token& arrow);

    /// Invoked to handle grouped type-specifiers.
    Type_specifier* on_paren_type(Type_specifier* ts,
                                  const Token& lparen,
                                  const Token& rparen);

    /// Invoked to analyze an assignment-expression.
    Expression* on_assignment_expression(Expression* e1,
                                         Expression* e2,
                                         const Token& op);
    
    /// Invoked to analyze an conditional-expression.
    Expression* on_conditional_expression(Expression* e1, 
                                          Expression* e2, 
                                          Expression* e3,
                                          const Token& question,
                                          const Token& colon);
    
    /// Invoked to analyze a logical-or-expression or logical-and-expression.
    Expression* on_logical_expression(Expression* e1, 
                                      Expression* e2,
                                      const Token& op);

    /// Invoked to analyze a logical not unary-expression.
    Expression* on_logical_expression(Expression* e,
                                      const Token& op);

    /// Invoked to analyze one of the bitwise-and|or|xor-expressions.
    Expression* on_bitwise_expression(Expression* e1, 
                                      Expression* e2,
                                      const Token& op);


    /// Invoked to analyze a bitwise no unary-expression.
    Expression* on_bitwise_expression(Expression* e, 
                                      const Token& op);

    /// Invoked to analyze an equality-expression.
    Expression* on_equality_expression(Expression* e1, 
                                       Expression* e2,
                                       const Token& op);

    /// Invoked to analyze a relational-expression.
    Expression* on_relational_expression(Expression* e1, 
                                         Expression* e2,
                                         const Token& op);
    
    /// Invoked to analyze a shift-expression.
    Expression* on_shift_expression(Expression* e1, 
                                    Expression* e2,
                                    const Token& op);
    
    /// Invoked to analyze an additive-expression.
    Expression* on_additive_expression(Expression* e1, 
                                       Expression* e2,
                                       const Token& op);

    /// Invoked to analyze a negation unary-expression.
    Expression* on_additive_expression(Expression* e,
                                       const Token& op);
    
    /// Invoked to analyze an multiplicative-expression.
    Expression* on_multiplicative_expression(Expression* e1, 
                                             Expression* e2,
                                             const Token& op);
    
    /// Invoked to analyze a reciprocal unary-expression.
    Expression* on_multiplicative_expression(Expression* e,
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
    
    /// Invoked to construct a unit literal.
    Expression* on_unit_literal(const Token& tok);

    /// Invoked to construct an integer literal.
    Expression* on_integer_literal(const Token& tok);

    /// Invoked to construct a boolean literal.
    Expression* on_boolean_literal(const Token& tok);

    /// Invoked to construct a floating point literal.
    Expression* on_float_literal(const Token& tok);

    /// Invoked to construct a character literal.
    Expression* on_character_literal(const Token& tok);

    /// Invoked to construct a string literal.
    Expression* on_string_literal(const Token& tok);

    /// Invoked to analyze an id-expression.
    Expression* on_id_expression(const Token& id);

    /// Invoked to construct an id-expression from a declaration.
    Expression* make_id_expression(Named_declaration* d);

    /// Invoked to construct an id-expression from a declaration.
    Expression* make_id_expression(Typed_declaration* d);

    /// Invoked to construct an init expression from a variable.
    Expression* make_init_expression(Variable_declaration* d);

    /// Invoked to analyze a paren-enclosed primary-expression.
    Expression* on_paren_expression(Expression* e,
                                    const Token& lparen, 
                                    const Token& rparen);

    /// Invoked to construt an empty block statement.
    Statement* on_start_block_statement();

    /// Invoked to finalize the block statement.
    Statement* on_finish_block_statement(Statement* s,
                                         Statement_seq&& ss,
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
    Declaration* on_data_identification(const Token& kw, const Token& id);

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
    
    /// Called to construct the compound-statement comprising the function 
    /// body. This declares paraemters within the outermost block to ensure
    /// we don't hide them unnecessarily.
    Declaration* on_start_function_definition(Declaration* d);

    /// Called to finalize the function definition.
    Declaration* on_finish_function_definition(Declaration* d, 
                                               Statement_seq&& ss,
                                               const Token& lbrace,
                                               const Token& rbrace);

    /// Called to analyze the declaration of a function parameter.
    Parameter* on_function_parameter(const Token& kind,
                                     const Token& id, 
                                     Type_specifier* type, 
                                     const Token& colon);

    /// Called to analyze the declaration of a function parameter.
    Parameter* on_function_parameter(const Token& kind, 
                                     Type_specifier* type, 
                                     const Token& colon);

    Parameter* on_variadic_parameter(const Token& ellipsis);
    Parameter* on_variadic_parameter(const Token& ellipsis, const Token& id);

    // Scope and declaration

    /// Make `d` the active declarative region.
    void enter_scope(Declaration* d);

    /// Make `s` the active declarative region.
    void enter_scope(Statement* s);

    /// Restore the previous declarative region. 
    void leave_scope(Declaration* d);

    /// Restore the previous declarative region. 
    void leave_scope(Statement* s);

    /// Restores the scope for a declaration.
    void restore_scope(Declaration* d);

    /// Empties the scope stack after a restoration.
    void empty_scope(Declaration* d);

    /// Returns current scoped declaration. This provides lookup and
    /// declaration facilities for the current declarative region.
    Scoped_declaration* get_current_declaration() { return m_decl; }

    /// Returns the current function declaration.
    Function_declaration* get_current_function();
    
    /// Returns the innermost block statement. Returns nullptr if not
    /// inside a function definition.
    Block_statement* get_current_block();

    // Declarations

    /// Register the declaration within its context.
    void identify(Named_declaration* d);

    /// Bind a declared name to its type. This performs additional checks
    /// to ensure that e.g., overloads are valid
    void declare(Typed_declaration* d);

    /// Bind a declaration to additional information.
    void declare(Named_declaration* d);

    // Lookup

    /// Perform unqualified lookup and return a set of declarations
    /// corresponding to the given name.
    Declaration_set unqualified_lookup(Symbol s);

    // Conversions

    /// Returns `e` converted to the type `t`.
    Expression* convert_to_type(Expression* e, Type* t);

    /// Returns `e` converted to a value (non-reference) type.
    Expression* convert_to_value(Expression* e);

    /// Returns `e` converted to a value of type `t`.
    Expression* convert_to_value(Expression* e, Type* t);

    /// Returns `e` converted to bool.
    Expression* convert_to_bool(Expression* e);

    // Integer conversions

    /// Returns `e` converted to the integer type `t`.
    Expression* convert_to_integer(Expression* e, Int_type* z);

    /// Converts an integer expression to a different integer type.
    Expression* convert_integer(Expression* e, Int_type* z);

    /// Extends an integer expression to a type of a greater rank.
    Expression* extend_integer(Expression* e, Int_type* z);
    
    /// Truncates an integer expression to a type of a lesser rank.
    Expression* truncate_integer(Expression* e, Int_type* z);

    // Floating point conversions

    /// Returns `e` converted to the floating point type `t`.
    Expression* convert_to_floating_point(Expression* e, Float_type* f);

    /// Converts a floating point expression to a different type.
    Expression* convert_floating_point(Expression* e, Float_type* f);

    /// Extends a floating point expression to one of a greater rank.
    Expression* extend_floating_point(Expression* e, Float_type* f);
    
    /// Truncates a floating point expression to one of a lesser rank.
    Expression* truncate_floating_point(Expression* e, Float_type* f);

    // Reference conversions

    /// Converts `e` to the reference type `t`.
    Expression* convert_to_reference(Expression* e, Reference_type* r);

    // Conversion to common type

    /// Converts `e1` and `e2` to a common type.
    Expression_pair convert_to_common_type(Expression* e1, Expression* e2);

    /// Converts expressions `e1` and `e2` to a common reference type.
    Expression_pair convert_to_common_reference(Expression* e1, Expression* e2);

    /// Converts expressions e1 and e2 to a common value type.
    Expression_pair convert_to_common_value(Expression* e1, Expression* e2);

    /// Converts integer expressions e1 and e2 to the integer type with the
    /// greatest rank.
    Expression_pair convert_to_common_integer(Expression* e1, Expression* e2);

    /// Converts floating point expressions e1 and e2 to the integer type 
    /// with the greatest rank.
    Expression_pair convert_to_common_floating_point(Expression* e1, Expression* e2);

    // Type requirements

    /// Checks that `e` has reference type.
    Expression* require_reference(Expression* e);

    /// Checks that `e` is a reference to an object of type `t`.
    Expression* require_reference_to(Expression* e, Type* t);

    /// Checks that `e` has integer type.
    Expression* require_integer(Expression* e);

    /// Checks that `e1` and `e2` have a common integer type.
    Expression_pair require_common_integer(Expression* e1, Expression* e2);

    /// Checks that `e1` and `e2` have the same value type.
    Expression_pair require_same_value(Expression* e1, Expression* e2);

    // Initialization

    /// Initialize the variable `d` with `e`.
    void default_initialize(Data_declaration* d);

    /// Initialize the variable `d` with `e`.
    void value_initialize(Data_declaration* d, Expression* e);
    void value_initialize_variable(Variable_declaration* d, Expression* e);
    void value_initialize_constant(Data_declaration* d, Expression* e);

  private:
    /// The translation context. This provides access to compiler resources.
    Context& m_cxt;

    /// The current scope.
    Scope* m_scope;

    /// The current declaration.
    Scoped_declaration* m_decl;
  };

} // namespace beaker
