#pragma once

#include <string>
#include <unordered_set>

namespace beaker
{
  /// Represents a symbol in the language. Because all symbols have unique
  /// values, the value of a symbol is its identity. 
  using Symbol = const std::string*;


  /// Ensures that all symbols having the same spelling are unique.
  class Symbol_table
  {
  public:
    /// Returns the symbol corresponding to string.
    Symbol get(const char* str) { return &*m_syms.insert(str).first; }
  
    /// Returns the symbol corresponding to string.
    Symbol get(const std::string& str) { return &*m_syms.insert(str).first; }

  private:
    std::unordered_set<std::string> m_syms;
  };


} // namespace beaker
