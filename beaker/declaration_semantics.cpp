#include "semantics.hpp"
#include "declaration.hpp"
#include "type.hpp"
#include "type_specifier.hpp"
#include "expression.hpp"
#include "initializer.hpp"
#include "statement.hpp"
#include "scope.hpp"
#include "context.hpp"
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

  // Allocate a declaration of the appropriate kind.
  static Data_declaration*
  make_data_decl(const Token& id, const Token& kw, Scoped_declaration* sd)
  {
    Symbol sym = id.get_symbol();
    Location loc = id.get_location();
    Location start = kw.get_location();
    switch (kw.get_name()) {
    default:
      __builtin_unreachable();
    case Token::val_kw:
      return new Value_declaration(sd, sym, start, loc);
    case Token::var_kw:
      return new Variable_declaration(sd, sym, start, loc);
    case Token::ref_kw:
      return new Reference_declaration(sd, sym, start, loc);
    }
  }

  Declaration* 
  Semantics::on_data_identification(const Token& id, const Token& kw)
  {
    // Create the entity.
    Data_declaration* data = make_data_decl(id, kw, m_decl);

    // Identify the declaration.
    identify(data);

    return data;
  }

  // Data declarations

  Declaration*
  Semantics::on_data_declaration(Declaration* d)
  {
    Data_declaration* data = static_cast<Data_declaration*>(d);

    // Set the type to auto.
    data->set_type(m_cxt.get_auto_type());

    // Declare the value or object.
    declare(data);

    return data;
  }

  Declaration* 
  Semantics::on_data_declaration(Declaration* d, Type_specifier* ts)
  {
    Data_declaration* data = static_cast<Data_declaration*>(d);

    // Set the type specifier for the declaration.
    data->set_type_specifier(ts);

    // Declare the value or object.
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
  Semantics::on_function_identification(const Token& id, const Token& kw)
  {
    // Create the entity.
    Symbol sym = id.get_symbol();
    Location loc = id.get_location();
    Location start = kw.get_location();
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

  Statement*
  Semantics::on_start_function_definition(Declaration* d)
  {
    Function_declaration* fn = static_cast<Function_declaration*>(d);

    // Create the statement that will serve as the outermost block
    // of the function; enter its scope.
    Block_statement* body = new Block_statement();
    enter_scope(body);

    // Identify (register) parameter declarations within the scope of the 
    // outermost block.
    for (Parameter *p : fn->get_parameters())
      identify(p);
    
    return body;
  } 

  Declaration*
  Semantics::on_finish_function_definition(Declaration* d, 
                                           Statement* s,
                                           const Token& lbrace,
                                           const Token& rbrace)
  {
    Function_declaration* fn = static_cast<Function_declaration*>(d);
    Block_statement* body = static_cast<Block_statement*>(s);
    
    // Leave the scope of the outermost block.
    leave_scope(body);

    // Update the brace locations.
    body->set_brace_locations(lbrace.get_location(), rbrace.get_location());

    // Update the function.
    fn->set_body(body);

    // FIXME: Perform final analysis of the function.
    // Check for return paths, etc.

    return fn;
  }

  Parameter*
  Semantics::on_function_parameter(const Token& id, 
                                   Type_specifier* type, 
                                   const Token& colon)
  {
    // Build underlying declaration.
    //
    // FIXME: Parameters can be variables or references.
    Symbol sym = id.get_symbol();
    Location loc = id.get_location();
    Value_declaration* val = new Value_declaration(m_decl, sym, loc, loc);
    val->set_type_specifier(type);

    // Build the parameter over the underlying declaration. 
    //
    // FIXME: When is the right time to set the depth and index?
    Parameter* parm = new Parameter(val);

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
