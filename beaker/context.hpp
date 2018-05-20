#pragma once

#include <beaker/common.hpp>
#include <beaker/symbol.hpp>

#include <memory>

namespace beaker
{
  class Unit_type;
  class Bool_type;
  class Int_type;
  class Float_type;
  class Auto_type;
  class Function_type;
  class Reference_type;

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
    Unit_type* get_unit_type();
    
    /// Returns the type `bool`.
    Bool_type* get_bool_type();
    
    /// Returns the type `int`.
    Int_type* get_int_type();

    /// Returns the type `float`.
    Float_type* get_float_type();

    /// Returns the type `auto`.
    Auto_type* get_auto_type();

    /// Returns the type `t&`.
    Reference_type* get_reference_type(Type* t);

    /// Returns the type `(t1, t2, ..., tn) -> tr`.
    Function_type* get_function_type(const Type_seq& ts, Type* r);

  private:
    /// The symbol table provides unique representations of symbols in the
    /// language. This is not used to associate information with identifiers.
    Symbol_table m_syms;

    // Types
    class Type_factory;

    /// Used to create (possibly unique) types.
    std::unique_ptr<Type_factory> m_types;

  };

} // namespace beaker