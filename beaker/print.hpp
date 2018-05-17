#pragma once

#include <beaker/common.hpp>

#include <iosfwd>

namespace beaker
{
  /// Provides information and facilities needed to pretty print language
  /// constructs.
  ///
  /// \todo This class is identical to dump context. It might be useful
  /// to factor out a base class (we expect printing flags to change).
  class Print_context
  {
  public:
    Print_context(std::ostream& os)
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

  void print(Print_context& dc, const Type* ts);
  void print(Print_context& dc, const Type_specifier* ts);

  std::ostream& operator<<(std::ostream& os, const Type& t);
  std::ostream& operator<<(std::ostream& os, const Type_specifier& t);

} // namespace beaker
