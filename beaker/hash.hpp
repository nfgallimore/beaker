#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <typeinfo>

// FIXME: Move this into a config header.

#if _WIN32 || _WIN64
#  if _WIN64
#    define BEAKER_ARCH_64
#  else
#    define BEAKER_ARCH_32
#  endif
#endif

#if __GNUC__
#  if __x86_64__ || __ppc64__
#    define BEAKER_ARCH_64
#  else
#    define BEAKER_ARCH_32
#  endif
#endif

namespace beaker
{
  namespace fnv1_detail 
  {

  constexpr std::uint32_t prime_32() { return 16777619u; }
  constexpr std::uint32_t basis_32() { return 2166136261u; }

  constexpr std::uint64_t prime_64() { return 1099511628211ul; }
  constexpr std::uint64_t basis_64() { return 14695981039346656037ul; }

  #if defined(BEAKER_ARCH_32)
  constexpr std::size_t prime() { return prime_32(); }
  constexpr std::size_t basis() { return basis_32(); }
  #elif defined(BEAKER_ARCH_64)
  constexpr std::size_t prime() { return prime_64(); }
  constexpr std::size_t basis() { return basis_64(); }
  #else 
  #  error Unknown target architecture
  #endif

  } // namespace fnv1_detail


  // The fnv1a hash algorithm is the default hasher.
  struct Hasher
  {
    Hasher()
      : code(fnv1_detail::basis())
    { }

    /// Hash bytes into the code.
    void operator()(const void* p, int n) 
    {
      unsigned char const* first = static_cast<unsigned char const*>(p);
      unsigned char const* limit = first + n;
      for ( ; first != limit; ++first)
        code = (code ^ *first) * fnv1_detail::prime();
    }

    /// Returns the computed hash code.
    operator std::size_t() const { return code; }

    std::size_t code;
  };


  /// Hash for trivially comparable T.
  template<typename T>
  inline typename std::enable_if_t<std::is_integral<T>::value, void>
  hash_append(Hasher& h, T t)
  {
    h(&t, sizeof(t));
  }

  template<typename T>
  inline typename std::enable_if_t<std::is_enum<T>::value, void>
  hash_append(Hasher& h, T t)
  {
    h(&t, sizeof(t));
  }

  /// Hash for floating point T. Guarantee that 0 and -0 have the same 
  /// hash code since 0 == -0.
  template<typename T>
  inline typename std::enable_if_t<std::is_floating_point<T>::value, void>
  hash_append(Hasher& h, T t)
  {
    if (t == 0)
      t = 0;
    h(&t, sizeof(t));
  }

  /// Hash for pointers. This just hashes the bits of the address.
  template<typename T>
  inline void
  hash_append(Hasher& h, const T* p)
  {
    h(&p, sizeof(p));
  }

  /// Hash append for nullptr.
  inline void
  hash_append(Hasher& h, std::nullptr_t p)
  {
    h(&p, sizeof(p));
  }

  /// Hash for type information.
  inline void
  hash_append(Hasher& h, const std::type_info& ti) 
  {
    hash_append(h, ti.hash_code());
  }

  // ------------------------------------------------------------------------ //
  // Hash functions

  /// Computes the hash values of objects.
  template<typename T>
  struct Hash
  {
    std::size_t operator()(const T& obj) const noexcept
    {
      Hasher h;
      hash_append(h, obj);
      return (std::size_t)h;
    };
  };


  /// Hashes a pointer to an object.
  template<typename T>
  struct Indirect_hash
  {
    std::size_t operator()(const T* obj) const noexcept
    {
      Hasher h;
      hash_append(h, *obj);
      return (std::size_t)h;
    };
  };

} // namespace beaker

