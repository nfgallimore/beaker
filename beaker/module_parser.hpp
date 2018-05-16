#pragma once

#include <beaker/parser.hpp>

#include <queue>
#include <vector>

namespace beaker
{
  /// Parses the top-level contents of a module. This is a multi-phase parse.
  class Module_parser : public Parser
  {
  public:
    Module_parser(Parse_context& cxt)
      : Parser(cxt)
    { }

    Declaration* parse_module();  

    Declaration* parse_declaration();
    Declaration_seq parse_declaration_seq();

    Declaration* parse_data_definition();
    void parse_data_type(Declaration* d);
    void parse_data_initializer(Declaration* d);
    
    Declaration* parse_function_definition();

    Parameter* parse_function_parameter();
    Parameter* parse_variadic_parameter();
    Parameter_seq parse_parameter_list();
    Parameter_seq parse_parameter_clause();

  private:
    void defer_data_type(Declaration* d, Token_seq&& toks);
    void defer_data_initializer(Declaration* d, Token_seq&& toks);
    void defer_function_signature(Declaration* d, Token_seq&& toks);
    void defer_function_definition(Declaration* d, Token_seq&& toks);

    void parse_deferred_declarations();
    void parse_deferred_definitions();
    void parse_deferred_actions(std::queue<Deferred_parse*>& q);

  private:
    /// A queue of deferred parsing actions, which is used to parse the
    /// types of functions and variables. This represents the second pass
    /// over the input after we have established the kind of each name.
    std::queue<Deferred_parse*> m_deferred_decls;

    /// A queue of deferred parsing actions, which is used to parse the
    /// definitions of functions and variables. This represents the third
    /// pass over the input after we have established the type of each name.
    std::queue<Deferred_parse*> m_deferred_defs;
  };


  /// The base class of all deferred parsing actions in module context.
  class Deferred_module_parse : public Deferred_parse
  {
  public:
    Deferred_module_parse(Parse_context& cxt, Declaration* d, Token_seq&& toks)
      : Deferred_parse(d, std::move(toks)), m_cxt(cxt)
    { }

  protected:
    /// The shared parse state.
    Parse_context& m_cxt;
  };


  /// Represents a parsing action for a deferred type of a variable or value.
  class Deferred_data_type : public Deferred_module_parse
  {
  public:
    using Deferred_module_parse::Deferred_module_parse;

    void parse() override;
  };


  /// Represents a parsing action for a deferred data initializer.
  class Deferred_data_initializer : public Deferred_module_parse
  {
  public:
    using Deferred_module_parse::Deferred_module_parse;

    void parse() override;
  };


  /// Represents a parsing action for a deferred function signature.
  class Deferred_function_signature : public Deferred_module_parse
  {
  public:
    using Deferred_module_parse::Deferred_module_parse;

    void parse() override;
  };


  /// Represents a parsing action for a deferred function definition.
  class Deferred_function_definition : public Deferred_module_parse
  {
  public:
    using Deferred_module_parse::Deferred_module_parse;

    void parse() override;
  };

} // namespace beaker
