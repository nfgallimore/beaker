#include "type.hpp"

#include <algorithm>

namespace beaker
{
  const char* 
  Type::get_kind_name() const
  {
    switch (m_kind) {
    case unit_kind: return "unit-type";
    case bool_kind: return "bool-type";
    case int_kind: return "int-type";
    case float_kind: return "float-type";
    case auto_kind: return "auto-type";
    case func_kind: return "function-type";
    case ref_kind: return "reference-type";
    }
  }

  static bool equal_types(const Type* t1, const Type* t2);

  static bool
  equal_function_types(const Function_type* t1, const Function_type* t2)
  {
    const Type_seq& p1 = t1->get_parameter_types();
    const Type_seq& p2 = t2->get_parameter_types();
    return equal_types(t1->get_return_type(), t2->get_return_type())
        && std::equal(p1.begin(), p1.end(), p2.begin(), p2.end(), equal_types);
  }

  static bool
  equal_reference_types(const Reference_type* t1, const Reference_type* t2)
  {
    return equal_types(t1->get_object_type(), t2->get_object_type());
  }

  static bool 
  equal_types(const Type* t1, const Type* t2)
  {
    if (t1 == t2)
      return true;
   
    if (t1->get_kind() != t2->get_kind())
      return false;
    
    switch (t1->get_kind()) {
    case Type::unit_kind:
    case Type::bool_kind:
    case Type::int_kind:
    case Type::float_kind:
    case Type::auto_kind:
      // FIXME: When are two auto's actually the same?
      return true;
    
    case Type::func_kind:
      return equal_function_types(static_cast<const Function_type*>(t1),
                                  static_cast<const Function_type*>(t2));
    case Type::ref_kind:
      return equal_reference_types(static_cast<const Reference_type*>(t1),
                                   static_cast<const Reference_type*>(t2));
    }
  }
  
  bool
  Type::is_equal_to(const Type& t) const
  {
    return equal_types(this, &t);
  }

  static void hash_type(Hasher& h, const Type* t);

  static void
  hash_int(Hasher& h, const Int_type* t)
  {
    hash_append(h, t->get_rank());
  }

  static void
  hash_float(Hasher& h, const Float_type* t)
  {
    hash_append(h, t->get_rank());    
  }

  static void
  hash_function(Hasher& h, const Function_type* t)
  {
    hash_append(h, t->get_return_type());
    const Type_seq& parms = t->get_parameter_types();
    std::for_each(parms.begin(), parms.end(), [&h](const Type* t) {
      hash_append(h, t);
    });
  }

  static void
  hash_reference(Hasher& h, const Reference_type* t)
  {
    hash_append(h, t->get_object_type());
  }

  static void
  hash_type(Hasher& h, const Type* t)
  {
    hash_append(h, t->get_kind());
    switch (t->get_kind()) {
    case Type::unit_kind:
    case Type::bool_kind:
    case Type::auto_kind:
      return;
    case Type::int_kind:
      return hash_int(h, static_cast<const Int_type*>(t));
    case Type::float_kind:
      return hash_float(h, static_cast<const Float_type*>(t));
    case Type::func_kind:
      return hash_function(h, static_cast<const Function_type*>(t));
    case Type::ref_kind:
      return hash_reference(h, static_cast<const Reference_type*>(t));
    }
  }

  std::size_t
  Type::hash() const
  {
    Hash<const Type*> h;
    return h(this);
  }

  void
  Type::hash(Hasher& h)
  {
    return hash_type(h, this);
  }

} // namespace beaker
