#pragma once

#include <beaker/common.hpp>

#include <iosfwd>

namespace beaker
{
  /// Provides information and facilities needed to dump abstract syntax
  /// tree nodes.
  class Dump_context
  {
  public:
    Dump_context(std::ostream& os)
      : m_os(os), m_indent(0)
    { }

    /// Returns the output stream.
    std::ostream& get_stream() const { return m_os; }

    /// Returns the indentation level.
    int get_indentation() const { return m_indent; }

    /// Increase the indentation level.
    void indent() { ++m_indent; }

    /// Decrease the indentation level.
    void undent() { --m_indent; }

  private:
    /// The output stream.
    std::ostream& m_os;

    /// The current indentation level.
    int m_indent;
  };

  void dump(Dump_context& dc, const Type* t);
  void dump(Dump_context& dc, const Expression* t);
  void dump(Dump_context& dc, const Statement* t);
  void dump(Dump_context& dc, const Declaration* d);

  void dump(const Type* t);
  void dump(const Expression* t);
  void dump(const Statement* t);
  void dump(const Declaration* d);

} // namespace beaker
