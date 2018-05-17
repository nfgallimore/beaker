#include "symbol.hpp"

#include <iostream>

namespace beaker
{
  std::ostream& 
  operator<<(std::ostream& os, Symbol sym)
  {
    return os << *sym;
  }

} // namespace beaker

