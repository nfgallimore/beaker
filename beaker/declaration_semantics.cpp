#include "semantics.hpp"
#include "declaration.hpp"
#include "type.hpp"
#include "type_specifier.hpp"
#include "expression.hpp"
#include "initializer.hpp"
#include "statement.hpp"
#include "scope.hpp"
#include "context.hpp"
#include "evaluation.hpp"
#include "print.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

namespace beaker
{
  /// Allocates the translation unit.
  Declaration*
  Semantics::on_start_translation()
  {
    Translation_unit* tu = new Translation_unit();

    // FIXME: Anything else to do here?

    return tu;
  }

  /// Performs any deferred semantic actions after all parsing has been.
  /// completed.
  Declaration*
  Semantics::on_finish_translation(Declaration* d)
  {
    return d;
  }

  // Create a value declaration without an introducer.
  static Data_declaration*
  make_value_decl(Semantics& sema, const Token& id)
  {
    Scoped_declaration* owner = sema.get_current_declaration();
    Symbol sym = id.get_symbol();
    Location loc = id.get_location();
    return new Value_declaration(owner, sym, loc, loc);
  }

  // Create a value declaration with an introducer.
  static Data_declaration*
  make_value_decl(Semantics& sema, const Token& intro, const Token& id)
  {
    Scoped_declaration* owner = sema.get_current_declaration();
    Location start = intro.get_location();
    Location loc = id.get_location();
    Symbol sym = id.get_symbol();
    return new Value_declaration(owner, sym, start, loc);
  }

  // Create a variable declaration with an introducer.
  static Data_declaration*
  make_variable_decl(Semantics& sema, const Token& intro, const Token& id)
  {
    Scoped_declaration* owner = sema.get_current_declaration();
    Location start = intro.get_location();
    Location loc = id.get_location();
    Symbol sym = id.get_symbol();
    return new Variable_declaration(owner, sym, start, loc);
  }

  // Create a reference declaration with an introducer.
  static Data_declaration*
  make_reference_decl(Semantics& sema, const Token& intro, const Token& id)
  {
    Scoped_declaration* owner = sema.get_current_declaration();
    Location start = intro.get_location();
    Location loc = id.get_location();
    Symbol sym = id.get_symbol();
    return new Reference_declaration(owner, sym, start, loc);
  }

  // Allocate a declaration of the appropriate kind.
  static Data_declaration*
  make_data_decl(Semantics& sema, const Token& intro, const Token& id)
  {
    switch (intro.get_name()) {
    default:
      break;
    case Token::val_kw:
      return make_value_decl(sema, intro, id);
    case Token::var_kw:
      return make_variable_decl(sema, intro, id);
    case Token::ref_kw:
      return make_reference_decl(sema, intro, id);
    }
    assert(false);
  }

  // Sets the adjusted type of the data declaration.
  static void
  set_data_type(Semantics& sema, Data_declaration* d, Type_specifier* ts)
  {
    // Set the type specifier.
    d->set_type_specifier(ts);

    // Set the adjusted type of the data declaration.
    Type* t = ts->get_type();
    if (auto* ref = dynamic_cast<Reference_declaration*>(d))
      t = sema.get_context().get_reference_type(t);
    d->set_type(t);
  }

  Declaration* 
  Semantics::on_data_identification(const Token& kind, const Token& id)
  {
    // Create the entity.
    Data_declaration* data = make_data_decl(*this, kind, id);

    // Identify the declaration.
    identify(data);

    return data;
  }

  // Data declarations

  /// Creates a data declaration with the type set to auto. The type specifier
  /// is omitted.
  Declaration*
  Semantics::on_data_declaration(Declaration* d)
  {
    Data_declaration* data = static_cast<Data_declaration*>(d);
    data->set_type(m_cxt.get_auto_type());
    declare(data);
    return data;
  }

  Declaration* 
  Semantics::on_data_declaration(Declaration* d, Type_specifier* ts)
  {
    Data_declaration* data = static_cast<Data_declaration*>(d);
    set_data_type(*this, data, ts);
    declare(data);
    return d;
  }

  Declaration* 
  Semantics::on_data_initialization(Declaration* d, 
                                    const Token& semi)
  {
    Data_declaration* data = static_cast<Data_declaration*>(d);
    default_initialize(data);
    return d;
  }

  Declaration* 
  Semantics::on_data_initialization(Declaration* d, 
                                    Expression* e, 
                                    const Token& semi)
  {
    Data_declaration* data = static_cast<Data_declaration*>(d);
    value_initialize(data, e);
    return d;
  }

  // Function declarations

  Declaration* 
  Semantics::on_function_identification(const Token& kw, const Token& id)
  {
    // Create the entity.
    Location start = kw.get_location();
    Location loc = id.get_location();
    Symbol sym = id.get_symbol();
    Function_declaration* fn = new Function_declaration(m_decl, sym, start, loc);

    // Identify the declaration
    identify(fn);

    return fn;
  }

  static Function_type*
  make_fn_type(Context& cxt, const Parameter_seq& parms, Type_specifier* ret)
  {
    Type_seq ptypes;
    for (Parameter *parm : parms)
      ptypes.push_back(parm->get_type());
    Type* rtype = ret->get_type();
    return cxt.get_function_type(std::move(ptypes), rtype);
  }

  Declaration*
  Semantics::on_function_declaration(Declaration* d, 
                                     Parameter_seq&& parms, 
                                     Type_specifier* ret, 
                                     const Token& lparen, 
                                     const Token& rparen, 
                                     const Token& arrow)
  {
    Function_declaration* fn = static_cast<Function_declaration*>(d);

    // Set the type.
    fn->set_type(make_fn_type(m_cxt, parms, ret));

    // Set the parameters and return specifier.
    fn->set_parameters(std::move(parms));
    fn->set_return(ret);
    
    return fn;
  }

  Declaration*
  Semantics::on_start_function_definition(Declaration* d)
  {
    Function_declaration* fn = static_cast<Function_declaration*>(d);

    // Create the function definition and make it the function body.
    // We're going to add statements later.
    Block_statement* body = new Block_statement();
    fn->set_body(body);
    
    // Enter block scope.
    enter_scope(body);

    // Identify (register) parameter declarations within the scope of the 
    // outermost block.
    for (Parameter *p : fn->get_parameters())
      identify(p);
    
    return fn;
  } 

  Declaration*
  Semantics::on_finish_function_definition(Declaration* d, 
                                           Statement_seq&& ss,
                                           const Token& lbrace,
                                           const Token& rbrace)
  {
    Function_declaration* fn = static_cast<Function_declaration*>(d);
    Block_statement* body = static_cast<Block_statement*>(fn->get_body());
    
    // Leave the scope of the outermost block.
    leave_scope(body);

    // Update the function definition.
    body->set_statements(std::move(ss));
    body->set_brace_locations(lbrace.get_location(), rbrace.get_location());

    // FIXME: Perform final analysis of the function.
    // Check for return paths, etc.

    return fn;
  }

  /// Constructs a parameter whose kind is determined by its introducer token.
  Parameter*
  Semantics::on_function_parameter(const Token& kind,
                                   const Token& id, 
                                   Type_specifier* type, 
                                   const Token& colon)
  {
    // Build underlying declaration.
    Data_declaration* inner = make_data_decl(*this, kind, id);
    set_data_type(*this, inner, type);

    // Build the parameter over the underlying declaration. 
    Parameter* parm = new Parameter(inner);

    // Identify and declare the parameter.
    identify(parm);
    declare(parm);

    return parm;
  }

  /// Constructs a value parameter.
  Parameter*
  Semantics::on_function_parameter(const Token& id, 
                                   Type_specifier* type, 
                                   const Token& colon)
  {
    // Build underlying declaration.
    Data_declaration* inner = make_value_decl(*this, id);
    set_data_type(*this, inner, type);

    // Build the parameter over the underlying declaration. 
    Parameter* parm = new Parameter(inner);

    // Identify and declare the parameter.
    identify(parm);
    declare(parm);

    return parm;
  }

  Parameter*
  Semantics::on_variadic_parameter(const Token& ellipsis)
  {
    __builtin_unreachable();
  }

  Parameter*
  Semantics::on_variadic_parameter(const Token& ellipsis, const Token& id)
  {
    __builtin_unreachable();
  }

  Declaration*
  Semantics::on_assertion(Expression* cond, const Token& kw, const Token& semi)
  {
    // The expression is converted to bool.
    cond = convert_to_bool(cond);

    // Create the assertion and add it to the owner.
    Scoped_declaration* owner = get_current_declaration();
    auto* decl = new Assertion(owner, cond, kw.get_location());
    owner->add_hidden_declaration(decl);

    // If we're not in block scope, evaluate the condition, possibly making
    // the program ill-formed.
    if (!get_current_block()) {
      Value result = evaluate(m_cxt, cond);
      if (!result.get_int())
        throw std::runtime_error("static assertion failed");
    }

    return decl;
  }

  // Declaration

  // Make a declaration available for name lookup.
  void
  Semantics::identify(Named_declaration* d)
  {
    // Check the current scope for an existing declaration.
    //
    // FIXME: Allow multiple declarations of functions, although we have
    // to check them at the point of declaration.
    Declaration_set decls = m_scope->lookup(d->get_name());
    if (!decls.is_empty()) {
      std::stringstream ss;
      ss << "redeclaration of " << *d->get_name();
      throw std::runtime_error(ss.str());
    }

    // Make the declaration available for lookup.
    m_scope->declare(d);
  }

  // Verify that the declaration is valid.
  void
  Semantics::declare(Typed_declaration* d)
  {
    // FIXME: If d is as function, check that it doesn't conflict with any
    // other overloads.
  }

  void
  Semantics::declare(Named_declaration* d)
  {
    // In general, we don't have to do anything here. However, if d is
    // a parameter, we should check its underlying declaration.
    //
    // FIXME: Do something interesting.
  }

} // namespace beaker
