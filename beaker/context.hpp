#pragma once

#include <beaker/common.hpp>
#include <beaker/symbol.hpp>

#include <memory>

namespace beaker
{
  /// Provides context (i.e., resources) to all major components of the
  /// compiler. This includes: memory allocation, diagnostics, memoization,
  /// internment, etc.
  ///
  /// \todo Integrate input management into the context.
  class Context
  {
  public:
    Context();
    ~Context();

    // Symbols

    /// Returns the symbol table.
    Symbol_table& get_symbol_table() { return m_syms; }

    /// Get a unique symbol for the given string.
    Symbol get_symbol(const char* str) { return m_syms.get(str); }
    
    /// Get a unique symbol for the given string.
    Symbol get_symbol(const std::string& str) { return m_syms.get(str); }

    // Types

    /// Returns the type `unit`.
    Type* get_unit_type() const { return m_unit_type.get(); }
    
    /// Returns the type `bool`.
    Type* get_bool_type() const { return m_bool_type.get(); }
    
    /// Returns the type `int`.
    Type* get_int_type() const { return m_int_type.get(); }

    /// Returns the type `float`.
    Type* get_float_type() const { return m_float_type.get(); }

    /// Returns the type `auto`.
    Type* get_auto_type() const { return m_auto_type.get(); }

    /// Returns a reference to the given type.
    Type* get_reference_type(Type* t);

  private:
    /// The symbol table provides unique representations of symbols in the
    /// language. This is not used to associate information with identifiers.
    Symbol_table m_syms;

    // Types
    //
    // FIXME: Add precise integer types and make `int` an alias to one of
    // those.

    /// The type `unit`.
    std::unique_ptr<Type> m_unit_type;

    /// The type `bool`.
    std::unique_ptr<Type> m_bool_type;

    /// The type `int`.
    std::unique_ptr<Type> m_int_type;

    /// The type `float`.
    std::unique_ptr<Type> m_float_type;

    /// The type `auto`.
    std::unique_ptr<Type> m_auto_type;
  };

} // namespace beaker