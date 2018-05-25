#include "dump.hpp"
#include "print.hpp"
#include "type.hpp"
#include "expression.hpp"
#include "conversion.hpp"
#include "initializer.hpp"
#include "statement.hpp"
#include "declaration.hpp"

#include <iostream>

namespace beaker
{
  // Fonts

  struct Font
  {
    static constexpr const char* normal = "\033[0m";

    // Plain fonts
    static constexpr const char* black = "\033[30m";
    static constexpr const char* red = "\033[31m";
    static constexpr const char* green = "\033[32m";
    static constexpr const char* yellow = "\033[33m";
    static constexpr const char* blue = "\033[34m";
    static constexpr const char* magenta = "\033[35m";
    static constexpr const char* cyan = "\033[36m";
    static constexpr const char* white = "\033[37m";

    // Bold fonts
    static constexpr const char* bold_black = "\033[1;30m";
    static constexpr const char* bold_red = "\033[1;31m";
    static constexpr const char* bold_green = "\033[1;32m";
    static constexpr const char* bold_yellow = "\033[1;33m";
    static constexpr const char* bold_blue = "\033[1;34m";
    static constexpr const char* bold_magenta = "\033[1;35m";
    static constexpr const char* bold_cyan = "\033[1;36m";
    static constexpr const char* bold_white = "\033[1;37m";
  };

  /// Indents on construction, undents on destruction.
  struct Indent_around
  {
    Indent_around(Dump_context& dc)
      : m_dc(dc)
    {
      m_dc.indent();
    }

    ~Indent_around()
    {
      m_dc.undent();
    }

    Dump_context& m_dc;
  };

  // Ensures that a newlinen is printed on destruction.
  struct Newline_finally
  {
    Newline_finally(Dump_context& dc)
      : m_dc(dc)
    { }

    ~Newline_finally()
    {
      m_dc.get_stream() << '\n';
    }

    Dump_context& m_dc;
  };

  static void
  dump_indentation(Dump_context& dc)
  {
    int level = dc.get_indentation();
    if (level == 0)
      return;

    // FIXME: Draw cool box lines. This is hard to do without knowing, id
    // advance how many children each node has. Right now we print this:
    //
    //    translation-unit
    //    └─decl-1
    //      └─type
    //        └─int-type
    //      └─init
    //    └─decl-2
    //      └─type
    //        └─int-type
    //    ...
    //
    // What we'd really like to do is drop lines to connect entries, and
    // make sure that the entry is properly closed.
    //
    //    translation-unit
    //    ├─decl-1
    //    │ ├─type
    //    │ │ └─int-type
    //    │ └─init
    //    └─decl-2
    //      └─type
    //        └─int-type
    //
    // Note that the lines stop after the last declaration.
    std::ostream& os = dc.get_stream();
    os << std::string(2 * (level - 1), ' ');
    os << "\u2514\u2500";
  }

  template<typename T>
  static void 
  dump_header(Dump_context& dc, const T* node)
  {
    dc.get_stream() << Font::bold_blue 
                    << node->get_kind_name() 
                    << Font::normal << ' ';
    
    dc.get_stream() << Font::green 
                    << (void*)node 
                    << Font::normal;
  }

  /// Emits the header for a node.
  template<typename T>
  static void 
  dump_node(Dump_context& dc, const T* node)
  {
    dump_indentation(dc);
    if (!node)
      dc.get_stream() << "<null>";
    else
      dump_header(dc, node);
  }

  /// Emits a label for a set of children.
  static void
  dump_label(Dump_context& dc, const char* label)
  {
    dump_indentation(dc);
    dc.get_stream() << Font::bold_black << label << Font::normal << '\n';
  }

  /// Emits a nested sequence of nodes.
  template<typename Seq>
  static void
  dump_sequence(Dump_context& dc, const Seq& seq)
  {
    for (const auto* elem : seq)
      dump(dc, elem);
  }

  // Types

  static void
  dump_attributes(Dump_context& dc, const Type* t)
  {
    Newline_finally nl(dc);
    
    if (!t)
      return;
    
    // FIXME: Implement me.
  }

  static void
  dump_children(Dump_context& dc, const Type* t)
  {
    if (!t)
      return;

    // FIXME: Implement me.
  }

  void 
  dump(Dump_context& dc, const Type* t)
  {
    dump_node(dc, t);
    dump_attributes(dc, t);
    dump_children(dc, t);
  }

  void
  dump(const Type* t)
  {
    Dump_context dc(std::cerr);
    dump(dc, t);
  }

  // Expressions

  static void
  dump_literal_attributes(Dump_context& dc, const Literal* e)
  {
    dc.get_stream() << " value=" << '\'' << e->get_token() << '\'';
  }

  static void
  dump_id_attributes(Dump_context& dc, const Id_expression* e)
  {
    if (Typed_declaration* td = e->get_declaration()) 
      dc.get_stream() << " decl=" << td->get_name() << " ref=" << (void*)td;
    else
      dc.get_stream() << " ref=<null>";
  }

  static void
  dump_attributes(Dump_context& dc, const Expression* e)
  {
    Newline_finally nl(dc);
    
    if (!e)
      return;
    
    if (Type* t = e->get_type())
      dc.get_stream() << " type=" << '\'' << *t << '\'';

    switch (e->get_kind()) {
    case Expression::bool_kind:
    case Expression::int_kind:
      return dump_literal_attributes(dc, static_cast<const Literal*>(e));
    
    case Expression::id_kind:
      return dump_id_attributes(dc, static_cast<const Id_expression*>(e));

    default:
      break;
    }
  }

  static void
  dump_unary_children(Dump_context& dc, const Unary_expression* e)
  {
    Indent_around indent(dc);
    dump(dc, e->get_operand());
  }

  static void
  dump_binary_children(Dump_context& dc, const Binary_expression* e)
  {
    Indent_around indent(dc);
    dump(dc, e->get_lhs());
    dump(dc, e->get_rhs());
  }

  static void
  dump_ternary_children(Dump_context& dc, const Ternary_expression* e)
  {
    Indent_around indent(dc);
    dump(dc, e->get_first());
    dump(dc, e->get_second());
    dump(dc, e->get_third());
  }

  static void
  dump_initializer_children(Dump_context& dc, const Value_initializer* e)
  {
    Indent_around indent(dc);
    dump(dc, e->get_value());
  }

  static void
  dump_children(Dump_context& dc, const Expression* e)
  {
    if (!e)
      return;

    // {
    //   Indent_around indent1(dc);
    //   dump_label(dc, "type");
    //   Indent_around indent2(dc);
    //   dump(dc, e->get_type());
    // }

    switch (e->get_kind()) {
    case Expression::bool_kind:
    case Expression::int_kind:
    case Expression::id_kind:
      // These nodes have no subexpressions.
      return;
    
    case Expression::neg_kind:
    case Expression::rec_kind:
    case Expression::bit_not_kind:
    case Expression::not_kind:
    case Expression::imp_conv:
      // Unary expressions
      return dump_unary_children(dc, static_cast<const Unary_expression*>(e));
    
    case Expression::and_kind:
    case Expression::or_kind:
      // Binary expressions
      return dump_binary_children(dc, static_cast<const Binary_expression*>(e));

    case Expression::cond_kind:
      // Ternary expressions.
      return dump_ternary_children(dc, static_cast<const Ternary_expression*>(e));

    case Expression::val_init:
      return dump_initializer_children(dc, static_cast<const Value_initializer*>(e));

    default:
      __builtin_unreachable();
    }
  }

  void 
  dump(Dump_context& dc, const Expression* e)
  {
    dump_node(dc, e);
    dump_attributes(dc, e);
    dump_children(dc, e);
  }

  void 
  dump(const Expression* e)
  {
    Dump_context dc(std::cerr);
    dump(dc, e);
  }

  // Statements

  static void
  dump_attributes(Dump_context& dc, const Statement* s)
  {
    Newline_finally nl(dc);
    
    if (!s)
      return;
    
    // FIXME: Implement me.
  }

  static void
  dump_block_children(Dump_context& dc, const Block_statement* s)
  {
    Indent_around indent(dc);
    for (Statement* ss : s->get_statements())
      dump(dc, ss);
  }

  static void
  dump_when_children(Dump_context& dc, const When_statement* s)
  {
    Indent_around indent(dc);
    dump(dc, s->get_condition());
    dump(dc, s->get_true_branch());
  }

  static void
  dump_if_children(Dump_context& dc, const If_statement* s)
  {
    Indent_around indent(dc);
    dump(dc, s->get_condition());
    dump(dc, s->get_true_branch());
    dump(dc, s->get_false_branch());
  }

  static void
  dump_while_children(Dump_context& dc, const While_statement* s)
  {
    Indent_around indent(dc);
    dump(dc, s->get_condition());
    dump(dc, s->get_body());
  }

  static void
  dump_return_children(Dump_context& dc, const Return_statement* s)
  {
    Indent_around indent(dc);
    dump(dc, s->get_return_value());
  }

  static void
  dump_expr_children(Dump_context& dc, const Expression_statement* s)
  {
    Indent_around indent(dc);
    dump(dc, s->get_expression());
  }

  static void
  dump_decl_children(Dump_context& dc, const Declaration_statement* s)
  {
    Indent_around indent(dc);
    dump(dc, s->get_declaration());
  }

  static void
  dump_children(Dump_context& dc, const Statement* s)
  {
    if (!s)
      return;

    switch (s->get_kind()) {
    case Statement::block_kind:
      return dump_block_children(dc, static_cast<const Block_statement*>(s));
    case Statement::when_kind:
      return dump_when_children(dc, static_cast<const When_statement*>(s));
    case Statement::if_kind:
      return dump_if_children(dc, static_cast<const If_statement*>(s));
    case Statement::while_kind:
      return dump_while_children(dc, static_cast<const While_statement*>(s));
    case Statement::ret_kind:
      return dump_return_children(dc, static_cast<const Return_statement*>(s));
    case Statement::expr_kind:
      return dump_expr_children(dc, static_cast<const Expression_statement*>(s));
    case Statement::decl_kind:
      return dump_decl_children(dc, static_cast<const Declaration_statement*>(s));
    default:
      break;
    }
  }

  void 
  dump(Dump_context& dc, const Statement* s)
  {
    dump_node(dc, s);
    dump_attributes(dc, s);
    dump_children(dc, s);
  }

  void 
  dump(const Statement* s)
  {
    Dump_context dc(std::cerr);
    dump(dc, s);
  }

  // Declarations

  static void 
  dump_translation_attributes(Dump_context& dc, const Translation_unit* d)
  {
  }

  static void
  dump_named_attributes(Dump_context& dc, const Named_declaration* d)
  {
    dc.get_stream() << " name=" << d->get_name();
  }

  static void
  dump_typed_attributes(Dump_context& dc, const Typed_declaration* d)
  {
    dump_named_attributes(dc, d);

    if (Type* t = d->get_type())
      dc.get_stream() << " type=" << *t;
  }

  static void 
  dump_function_attributes(Dump_context& dc, const Function_declaration* d)
  {
    dump_typed_attributes(dc, d);
  }

  static void 
  dump_data_attributes(Dump_context& dc, const Data_declaration* d)
  {
    dump_typed_attributes(dc, d);
  }

  static void 
  dump_parameter_attributes(Dump_context& dc, const Parameter* d)
  {
    dc.get_stream() << " depth=" << d->get_depth()
                    << " index=" << d->get_index();
  }

  // Emit named attributes after the node header.
  static void
  dump_attributes(Dump_context& dc, const Declaration* d)
  {
    Newline_finally nl(dc);
    
    if (!d)
      return;
    
    switch (d->get_kind()) {
    case Declaration::tu_kind:
      dump_translation_attributes(dc, static_cast<const Translation_unit*>(d));
      break;
    case Declaration::func_kind:
      dump_function_attributes(dc, static_cast<const Function_declaration*>(d));
      break;
    case Declaration::val_kind:
    case Declaration::var_kind:
    case Declaration::ref_kind:
      dump_data_attributes(dc, static_cast<const Data_declaration*>(d));
      break;
    case Declaration::parm_kind:
      dump_parameter_attributes(dc, static_cast<const Parameter*>(d));
      break;
    }
  }

  static void
  dump_translation_children(Dump_context& dc, const Translation_unit* d)
  {
    Indent_around nest(dc);
    dump_sequence(dc, d->get_declarations());
  }
  
  static void
  dump_typed_children(Dump_context& dc, const Typed_declaration* d)
  {
    Indent_around nest(dc);
    dump(dc, d->get_type());
  }

  static void
  dump_function_children(Dump_context& dc, const Function_declaration* d)
  {
    Indent_around nest(dc);

    dump_label(dc, "type");
    dump_typed_children(dc, d);
    
    dump_label(dc, "parameters");
    {
      Indent_around parms(dc);
      dump_sequence(dc, d->get_parameters());
    }
    
    dump_label(dc, "body");
    {
      Indent_around body(dc);
      dump(dc, d->get_body());
    }
  }

  static void
  dump_data_children(Dump_context& dc, const Data_declaration* d)
  {
    Indent_around indent(dc);
    
    dump_label(dc, "type");
    dump_typed_children(dc, d);
    
    dump_label(dc, "initializer");
    {
      Indent_around indent(dc);
      dump(dc, d->get_initializer());
    }
  }

  static void
  dump_parameter_children(Dump_context& dc, const Parameter* d)
  {
    Indent_around nested(dc);
    dump(dc, d->get_declaration());
  }

  // Emit the children of a declaration.
  static void
  dump_children(Dump_context& dc, const Declaration* d)
  {
    if (!d)
      return;

    switch (d->get_kind()) {
    case Declaration::tu_kind:
      dump_translation_children(dc, static_cast<const Translation_unit*>(d));
      break;
    case Declaration::func_kind:
      dump_function_children(dc, static_cast<const Function_declaration*>(d));
      break;
    case Declaration::val_kind:
    case Declaration::var_kind:
    case Declaration::ref_kind:
      dump_data_children(dc, static_cast<const Data_declaration*>(d));
      break;
    case Declaration::parm_kind:
      dump_parameter_children(dc, static_cast<const Parameter*>(d));
      break;
    }
  }

  void 
  dump(Dump_context& dc, const Declaration* d)
  {
    dump_node(dc, d);
    dump_attributes(dc, d);
    dump_children(dc, d);
  }

  void 
  dump(const Declaration* d)
  {
    Dump_context dc(std::cerr);
    dump(dc, d);
  }

  void 
  dump(const Scoped_declaration* d)
  {
    dump(d->cast_as_declaration());
  }

} // namespace beaker
