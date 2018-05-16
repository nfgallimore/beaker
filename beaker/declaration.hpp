#pragma once

#include <beaker/common.hpp>
#include <beaker/token.hpp>

#include <unordered_map>

namespace beaker
{
  class Declaration;
  class Scoped_declaration;
  class Named_declaration;
  class Function_type;


  /// Associates names with declarations.
  ///
  /// \todo A name is not just a symbol.
  using Declaration_map = std::unordered_multimap<Symbol, Named_declaration*>;


  /// A set of declarations having the same name, but possibly denoting
  /// different entities.
  class Declaration_set
  {
  public:
    using iterator = Declaration_map::iterator;

    Declaration_set()
      : m_first(), m_last()
    { }

    Declaration_set(iterator f, iterator l)
      : m_first(f), m_last(l)
    { }

    /// Returns true if the set is empty.
    bool empty() const { return m_first == m_last; }

    /// Returns true if this is a singleton set.
    bool singleton() const { return !empty() && std::next(m_first) == m_last; }

    /// Returns an iterator to the first declaration.
    iterator begin() const { return m_first; }

    /// Returns an iterator past the last declaration.
    iterator end() const { return m_last; }

  private:
    iterator m_first;
    iterator m_last;
  };
  

  /// The base class of all declarations.
  class Declaration
  {
  public:
    enum Kind
    {
      tu_kind,
      func_kind,
      val_kind,
      var_kind,
      parm_kind,
    };

  protected:
    /// Construct a declaration of kind `k` in the scoped declaration.
    Declaration(Kind k, Scoped_declaration* sd)
      : m_kind(k), m_scope(sd)
    { }

  public:
    virtual ~Declaration() = default;

    // Kind

    /// Returns the kind of declaration.
    Kind get_kind() const { return m_kind; }

    /// Returns true if this is a translation unit.
    bool is_translation_unit() const { return m_kind == tu_kind; }

    /// Returns true if this is a function.
    bool is_function() const { return m_kind == func_kind; }

    /// Returns true if this is a data definition.
    bool is_data() const { return is_value() || is_variable(); }

    /// Returns true if this is a value definition.
    bool is_value() const { return m_kind == val_kind; }

    /// Returns true if this is a variable definition.
    bool is_variable() const { return m_kind == var_kind; }

    /// Returns true if this is a scoped declaration.
    bool is_scoped() const;

    // Scope

    /// Returns the scope (declaration) in which this is declared.
    Scoped_declaration* get_scope() const { return m_scope; }

    /// Returns this as a scoped declaration, or nullptr if it is not.
    Scoped_declaration* get_as_scoped() const;

    /// Returns 
    Scoped_declaration* cast_as_scoped() const;

    /// Returns the start location of the this statement.
    virtual Location get_start_location() const { return Location(); }
    
    /// Returns the end location of the this statement.
    virtual Location get_end_location() const { return Location(); }

  private:
    /// The kind of declaration.
    Kind m_kind;

    /// The region of source text associated with the declaration.
    Scoped_declaration* m_scope;
  };


  /// Provides information about a nested sequence of declarations within
  /// a declarative region. Declarative regions are associated with some
  /// kinds of declarations and with some kinds of statements.
  ///
  /// Note that this is outside of the declaration hierarchy. Only certain
  /// declarations derive from this class.
  class Scoped_declaration
  {
  public:
    // Kind
    using Kind = Declaration::Kind;

    Scoped_declaration(Declaration* d)
      : m_derived(d)
    { }

    /// Returns this object cast as a declaration.
    Declaration* cast_as_declaration() const { return m_derived; }

    /// Returns the kind of declaration.
    Kind get_decl_kind() const { return m_derived->get_kind(); }

    /// Returns true if this is a translation unit.
    bool is_translation_unit() const { m_derived->is_translation_unit(); }

    /// Returns true if this is a function.
    bool is_function() const { return m_derived->is_function(); }

    // Context

    /// Returns the parent context.
    Scoped_declaration* get_parent() const { return m_derived->get_scope(); }

    // Nested declarations

    /// Adds a declaration to the context, making it available for lookup.
    void add_visible_declaration(Named_declaration* d);

    /// Adds the declaration to the list of nested declarations.
    void add_hidden_declaration(Declaration* d);

    /// Returns the declaration of a name.
    Declaration_set lookup(Symbol sym) const;

  private:
    /// A pointer to the derived class object.
    Declaration* m_derived;
  
    /// The sequence of declarations.
    Declaration_seq m_decls;

    /// A facility to support name lookup.
    Declaration_map m_lookup;
  };


  /// Represents a translation unit, comprised of one or more source files.
  /// This is the root of the declaration containment hierarchy.
  class Translation_unit : public Declaration, public Scoped_declaration
  {
  public:
    /// Constructs the translation unit.
    Translation_unit()
      : Declaration(tu_kind, nullptr), Scoped_declaration(this)
    { }
  };


  /// The base class of all declarations that introduce names.
  ///
  /// \todo The name should be a syntax tree, not just an identifier.
  class Named_declaration : public Declaration
  {
  protected:
    Named_declaration(Kind k, Scoped_declaration* sd, const Token& id)
     : Declaration(k, sd), m_id(id)
    { assert(id.is(Token::identifier)); }

  public:
    /// Returns the name of the symbol.
    Symbol get_name() const { return m_id.get_symbol(); }

    /// Returns the declared identifier.
    const Token& get_identifier() const { return m_id; }

  private:
    Token m_id;
  };


  /// The base class of all declarations that have an associated type. Note
  /// that type specifiers are associated with the derived classes, not this
  /// class.
  class Typed_declaration : public Named_declaration
  {
  protected:
    Typed_declaration(Kind k, Scoped_declaration* sd, const Token& id)
      : Named_declaration(k, sd, id), m_type()
    { }

  public:
    /// Returns the type of the declaration.
    Type* get_type() const { return m_type; }

    /// Sets the type of the declaration.
    void set_type(Type* t) { assert(!m_type); m_type = t; }

  private:
    Type* m_type;
  };


  /// Represents the declaration of a function.
  class Function_declaration : public Typed_declaration, public Scoped_declaration
  {
  public:
    Function_declaration(Scoped_declaration* sd, const Token& id)
      : Typed_declaration(func_kind, sd, id), Scoped_declaration(this)
    { }

    /// Returns the type of the function.
    Function_type* get_type() const;

    /// Sets the type of the function.
    void set_type(Function_type* t);

    /// Returns the return type of the function.
    Type* get_return_type() const;

    /// Returns the parameter types of the function.
    const Type_seq& get_parameter_types() const;
    
    /// Returns the parameters.
    Type_seq& get_parameter_types();

    /// Returns the parameters.
    const Parameter_seq& get_parameters() const { return m_parms; }
    
    /// Returns the parameters.
    Parameter_seq& get_parameters() { return m_parms; }

    /// Sets the parameters.
    void set_parameters(Parameter_seq&& parms) { m_parms = std::move(parms); }

    /// Returns the return type specifier.
    Type_specifier* get_return() const { return m_ret; }

    /// Sets the return type specifier.
    void set_return(Type_specifier* ts) { m_ret = ts; }

  private:
    /// The parameters of the function.
    Parameter_seq m_parms;

    /// The return type specifier.
    Type_specifier* m_ret;
  };


  /// The base class of all data declarations. This class provides an optional
  /// type specifier to indicate the declared type of the value or object.
  class Data_declaration : public Typed_declaration
  {
  protected:
    Data_declaration(Kind k, Scoped_declaration* sd, const Token& id)
      : Typed_declaration(k, sd, id), m_ts()
    { }

  public:
    /// Returns the type specifier for the declaration.
    Type_specifier* get_type_specifier() const { return m_ts; }

    /// Sets the type specifier for the declaration. Note that this
    /// will also update the type.
    void set_type_specifier(Type_specifier* ts);

  private:
    /// The type specifier of data declaration. This may be omitted.
    Type_specifier* m_ts;
  };


  /// Represents the declaration of a value. Value declarations introduce 
  /// names that bind to expressions, which compute values.
  class Value_declaration : public Data_declaration
  {
  public:
    Value_declaration(Scoped_declaration* sd, const Token& id)
      : Data_declaration(val_kind, sd, id)
    { }
  };


  /// Represents the declaration of a variable. Variable declarations introduce
  /// names that bind to objects, which hold values.
  class Variable_declaration : public Data_declaration
  {
  public:
    Variable_declaration(Scoped_declaration* sd, const Token& id)
      : Data_declaration(var_kind, sd, id)
    { }
  };


  /// A parameter introduces a declaration as part of a mapping (function
  /// or template). This owns the underlying declaration and provides 
  /// additional information. Note that parameters can be type parameters,
  /// so the most general base class is going to be named (not typed).
  ///
  /// \todo Add depth and position information? We would need to to support
  /// alpha equivalence a la C++.
  class Parameter : public Named_declaration
  {
  public:
    Parameter(Named_declaration* d);

    /// Returns the declared parameter.
    Named_declaration* get_declaration() const { return m_decl; }

    /// Return the type of the declaration, if typed.
    Type* get_type() const;

  private:
    Named_declaration* m_decl;
  };

  inline Type*
  Parameter::get_type() const
  {
    assert(dynamic_cast<const Typed_declaration*>(m_decl));
    return static_cast<const Typed_declaration*>(m_decl)->get_type();
  }

} // namespace beaker
