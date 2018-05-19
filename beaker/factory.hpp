#pragma once

#include <forward_list>
#include <unordered_set>

namespace beaker
{
  /// Creates objects of a particular type. All objects are destroyed when
  /// the factory goes out of scope.
  template<typename T>
  class Basic_factory : private std::forward_list<T>
  {
  public:
    template<typename... Args>
    T* make(Args&&... args)
    {
      this->emplace_back(std::forward<Args>(args)...);
      return &this->back();
    }
  };


  /// Creates objects of a particular, ensuring that each object with unique
  /// properties is created exactly once. In other words, this "interns" or
  /// "canonicalizes" the objects.
  template<typename T, typename Hash, typename Eq>
  struct Unique_factory : std::unordered_set<T, Hash, Eq>
  {
    template<typename... Args>
    T* make(Args&&... args)
    {
      const T& elem = *this->emplace(std::forward<Args>(args)...).first;
      return const_cast<T*>(&elem);
    }
  };

} // namespace beaker
