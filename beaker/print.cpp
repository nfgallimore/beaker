#include "print.hpp"
#include "type.hpp"
#include "type_specifier.hpp"

#include <iostream>

namespace beaker
{
  // Helper functions
  namespace
  {
    /// Prints a textual literal.
    static void
    print_literal(Print_context& pc, const char* str)
    {
      pc.get_stream() << str;
    }

    /// Prints a comma separated list of terms.
    template<typename Seq>
    void
    print_list(Print_context& pc, const Seq& seq)
    {
      for (auto iter = seq.begin(); iter != seq.end(); ++iter) {
        print(pc, *iter);
        if (std::next(iter) != seq.end())
          pc.get_stream() << ',' << ' ';
      }
    }

    /// A guard class; prints an opening an closing character on
    /// construction and destruction.
    struct Enclosure
    {
      Enclosure(Print_context& pc, char open, char close)
        : m_pc(pc)
      {
        m_pc.get_stream() << m_open;
      }

      ~Enclosure()
      {
        m_pc.get_stream() << m_close;
      }

      Print_context& m_pc;
      char m_open;
      char m_close;
    };

    /// Prints parens.
    struct Paren_enclosure : Enclosure
    {
      Paren_enclosure(Print_context& pc)
        : Enclosure(pc, '(', ')')
      { }
    };

  } // namespace

  // Types

  static void
  print_function_type(Print_context& pc, const Function_type* t)
  {
    {
      Paren_enclosure enc(pc);
      print_list(pc, t->get_parameter_types());
    }
    pc.get_stream() << " -> ";
    print(pc, t->get_return_type());
  }

  static void
  print_reference_type(Print_context& pc, const Reference_type* t)
  {
    // FIXME: We may need to enclose the object type in parens.
    print(pc, t->get_object_type());
    pc.get_stream() << '&';
  }

  void
  print(Print_context& pc, const Type* t)
  {
    switch (t->get_kind()) {
    case Type::unit_kind:
      return print_literal(pc, "unit");
    case Type::bool_kind:
      return print_literal(pc, "bool");
    case Type::int_kind:
      return print_literal(pc, "int");
    case Type::float_kind:
      return print_literal(pc, "float");
    case Type::auto_kind:
      return print_literal(pc, "auto");
    case Type::func_kind:
      return print_function_type(pc, static_cast<const Function_type*>(t));
    case Type::ref_kind:
      return print_reference_type(pc, static_cast<const Reference_type*>(t));
    }
  }

  // Type specifiers

  void
  print(Print_context& pc, const Type_specifier* ts)
  {
    // FIXME: This is kind of cheating.
    print(pc, ts->get_type());
  }

  // Streaming

  std::ostream& 
  operator<<(std::ostream& os, const Type& t)
  {
    Print_context pc(os);
    print(pc, &t);
    return os;
  }

  std::ostream& 
  operator<<(std::ostream& os, const Type_specifier& ts)
  {
    Print_context pc(os);
    print(pc, &ts);
    return os;
  }

} // namespace beaker
