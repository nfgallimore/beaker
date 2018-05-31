#include "declaration.hpp"
#include "type.hpp"
#include "type_specifier.hpp"
#include "dump.hpp"

#include <iostream>
  
namespace beaker
{
  Declaration_set
  Declaration_map::lookup(Symbol sym) const 
  {
    auto& map = const_cast<Declaration_map&>(*this);
    auto range = map.equal_range(sym);
    return Declaration_set(range.first, range.second);
  }
  
  void
  Declaration_map::declare(Named_declaration* d)
  {
    emplace(d->get_name(), d);
  }

  const char*
  Declaration::get_kind_name() const
  {
    switch (m_kind) {
    case tu_kind: return "translation-unit";
    case func_kind: return "function-declaration";
    case val_kind: return "value-declaration";
    case var_kind: return "variable-declaration";
    case ref_kind: return "reference-declaration";
    case parm_kind: return "parameter";
    default:
      break;
    }
    __builtin_unreachable();
  }

  bool 
  Declaration::is_scoped() const
  {
    switch (m_kind) {
    case tu_kind:
    case func_kind:
      return true;
    default:
      return false;
    }
  }

  Scoped_declaration*
  Declaration::get_as_scoped() const
  {
    if (is_scoped())
      return cast_as_scoped();
    else
      return nullptr;
  }

  Scoped_declaration*
  Declaration::cast_as_scoped() const
  {
    assert(is_scoped());
    Declaration* self = const_cast<Declaration*>(this);
    switch (m_kind) {
    case tu_kind:
      return static_cast<Translation_unit*>(self);
    case func_kind:
      return static_cast<Function_declaration*>(self);
    default:
      __builtin_unreachable();
    }
  }

  Declaration*
  Declaration::get_enclosing_declaration() const
  {
    return m_scope ? m_scope->cast_as_declaration() : nullptr;
  }

  void
  Declaration::dump() const
  {
    Dump_context dc(std::cerr);
    beaker::dump(dc, this);
  }

  void
  Scoped_declaration::add_visible_declaration(Named_declaration* d)
  {
    assert(m_lookup.find(d->get_name()) == m_lookup.end());
    
    // Add the declaration for lookup.
    m_lookup.declare(d);
    
    // Add the declaration to the list.
    add_hidden_declaration(d);
  }

  void
  Scoped_declaration::add_hidden_declaration(Declaration* d)
  {
    m_decls.push_back(d);
  }

  Declaration_set
  Scoped_declaration::lookup(Symbol sym) const 
  {
    return m_lookup.lookup(sym);
  }

  Function_type*
  Function_declaration::get_type() const
  {
    return static_cast<Function_type*>(Typed_declaration::get_type());
  }

  void 
  Function_declaration::set_type(Function_type* t)
  {
    Typed_declaration::set_type(t);
  }

  Type*
  Function_declaration::get_return_type() const
  {
    return get_type()->get_return_type();
  }

  const Type_seq&
  Function_declaration::get_parameter_types() const
  {
    return get_type()->get_parameter_types();
  }

  Type_seq&
  Function_declaration::get_parameter_types()
  {
    return get_type()->get_parameter_types();
  }

  void 
  Data_declaration::set_initializer(Expression* e)
  {
    assert(!m_init);
    m_init = e;
  }

  bool
  Data_declaration::has_static_storage() const
  {
    // FIXME: Namespaces can also have static storage.
    return get_enclosing_declaration()->is_translation_unit();
  }

  bool
  Data_declaration::has_automatic_storage() const
  {
    return get_enclosing_declaration()->is_function();
  }

  Parameter::Parameter(Named_declaration* d)
    : Named_declaration(parm_kind, 
                        d->get_owner(), 
                        d->get_name(), 
                        d->get_start_location(), 
                        d->get_name_location()), 
      m_decl(d),
      m_depth(0),
      m_index(0)
  { }

} // namespace beaker
