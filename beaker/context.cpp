#include "context.hpp"
#include "type.hpp"
#include "factory.hpp"

namespace beaker
{
  struct Type_equal
  {
    bool 
    operator()(const Type& t1, const Type& t2) const noexcept
    {
      return t1.is_equal_to(t2);
    }
  };

  /// A general facility for hashing trees.
  template<typename T>
  struct Tree_hash
  {
    std::size_t operator()(const T& t) const noexcept
    {
      return t.hash();
    }
  };

  template<typename T>
  using Unique_type = Unique_factory<T, Tree_hash<T>, Type_equal>;

  class Context::Type_factory
  {
  public:
    Type_factory()
      : unit_type(new Unit_type()),
        bool_type(new Bool_type()),
        int_type(new Int_type(Int_type::int32)),
        float_type(new Float_type(Float_type::float32)),
        auto_type(new Auto_type())
    { }

    /// The type `unit`.
    std::unique_ptr<Unit_type> unit_type;

    /// The type `bool`.
    std::unique_ptr<Bool_type> bool_type;

    /// The type `int`.
    ///
    /// \todo Support an extended integer type family.
    std::unique_ptr<Int_type> int_type;

    /// The type `float`.
    ///
    /// \todo Support an extended integer type family.
    std::unique_ptr<Float_type> float_type;

    /// The type `auto`.
    std::unique_ptr<Auto_type> auto_type;

    /// Creates unique function types.
    Unique_type<Function_type> function_types;

    /// Creates unique reference types.
    Unique_type<Reference_type> reference_types;
  };

  Context::Context()
    : m_syms(),
      m_types(new Type_factory())
  { }

  Context::~Context()
  { }

  Unit_type*
  Context::get_unit_type()
  { 
    return m_types->unit_type.get(); 
  }
  
  Bool_type*
  Context::get_bool_type()
  { 
    return m_types->bool_type.get(); 
  }
  
  Int_type*
  Context::get_int_type()
  { 
    return m_types->int_type.get(); 
  }

  Float_type*
  Context::get_float_type()
  { 
    return m_types->float_type.get(); 
  }

  Auto_type*
  Context::get_auto_type()
  { 
    return m_types->auto_type.get(); 
  }

  Reference_type*
  Context::get_reference_type(Type* t)
  {
    return m_types->reference_types.make(t);
  }

  Function_type*
  Context::get_function_type(const Type_seq& ts, Type* t)
  {
    return m_types->function_types.make(ts, t);
  }

} // namespace beaker
