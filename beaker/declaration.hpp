#pragma once

#include <beaker/common.hpp>
#include <beaker/symbol.hpp>
#include <beaker/location.hpp>

#include <unordered_map>

namespace beaker
{
  class Declaration_set;
  class Declaration_map;
  class Declaration;
  class Scoped_declaration;
  class Named_declaration;
  class Function_type;


  /// Associates names with declarations.
  ///
  /// \todo A name is not just a symbol.
  class Declaration_map : public std::unordered_multimap<Symbol, Named_declaration*>
  {
    using Base = std::unordered_multimap<Symbol, Named_declaration*>;
  public:
    using Base::Base;

    /// Returns the set of declarations in the lookup map.
    Declaration_set lookup(Symbol sym) const;

    /// Adds d to the map.
    void declare(Named_declaration* d);
  };


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
    bool is_empty() const { return m_first == m_last; }

    /// Returns true if this is a singleton set.
    bool is_singleton() const { return !is_empty() && std::next(m_first) == m_last; }

    /// Returns the only found decalration. Use this only when a single
    /// declaration is expected. Otherwise, the iterator interface should
    /// be used to traverse the list of declarations.
    Named_declaration* get_single_declaration() const;

    /// Returns an iterator to the first declaration.
    iterator begin() const { return m_first; }

    /// Returns an iterator past the last declaration.
    iterator end() const { return m_last; }

  private:
    iterator m_first;
    iterator m_last;
  };

  inline Named_declaration* 
  Declaration_set::get_single_declaration() const
  {
    assert(is_singleton());
    return m_first->second;
  }
  

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
      ref_kind,
      parm_kind,
      assert_kind,
    };

  protected:
    /// Construct a declaration of kind `k` in the scoped declaration.
    Declaration(Kind k, Scoped_declaration* sd, Location start)
      : m_kind(k), m_scope(sd), m_start(start)
    { }

  public:
    virtual ~Declaration() = default;

    // Kind

    /// Returns the kind of declaration.
    Kind get_kind() const { return m_kind; }

    /// Returns a textual representation of the kind.
    const char* get_kind_name() const;

    /// Returns true if this is a translation unit.
    bool is_translation_unit() const { return m_kind == tu_kind; }

    /// Returns true if the declaration is typed, meaning that it has an
    /// associated value.
    bool is_typed() const { return is_function() || is_data(); }

    /// Returns true if this is a function.
    bool is_function() const { return m_kind == func_kind; }

    /// Returns true if this is a data definition.
    bool is_data() const { return is_value() || is_variable() || is_reference(); }

    /// Returns true if this is a value definition.
    bool is_value() const { return m_kind == val_kind; }

    /// Returns true if this is a variable definition.
    bool is_variable() const { return m_kind == var_kind; }

    /// Returns true if this is a reference definition.
    bool is_reference() const { return m_kind == ref_kind; }

    /// Returns true if this is a scoped declaration.
    bool is_scoped() const;

    // Scope

    /// Returns the scoped declaration in which this is declared.
    Scoped_declaration* get_owner() const { return m_scope; }

    /// Returns the declaration in which this is declared.
    Declaration* get_enclosing_declaration() const;

    /// Returns this as a scoped declaration, or nullptr if it is not.
    Scoped_declaration* get_as_scoped() const;

    /// Returns this as a scoped declaration.
    Scoped_declaration* cast_as_scoped() const;

    // Physical location.

    /// Returns the start location of the this statement.
    virtual Location get_start_location() const { return m_start; }
    
    /// Returns the end location of the this statement.
    virtual Location get_end_location() const { return Location(); }

    // Debugging

    // Emit a representation of the AST.
    void dump() const;

  private:
    /// The kind of declaration.
    Kind m_kind;

    /// The region of source text associated with the declaration.
    Scoped_declaration* m_scope;

    /// The starting location of the declaration.
    Location m_start;
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
    bool is_translation_unit() const { return m_derived->is_translation_unit(); }

    /// Returns true if this is a function.
    bool is_function() const { return m_derived->is_function(); }

    // Context

    /// Returns the parent context.
    Scoped_declaration* get_parent() const { return m_derived->get_owner(); }

    // Nested declarations

    /// Returns the list of nested declarations.
    const Declaration_seq& get_declarations() const { return m_decls; }

    /// Returns the list of nested declarations.
    Declaration_seq& get_declarations() { return m_decls; }

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
      : Declaration(tu_kind, nullptr, Location()), Scoped_declaration(this)
    { }
  };


  /// The base class of all declarations that introduce names.
  ///
  /// \todo The name should be a syntax tree, not just an identifier.
  class Named_declaration : public Declaration
  {
  protected:
    Named_declaration(Kind k, 
                      Scoped_declaration* sd, 
                      Symbol sym, 
                      Location start, 
                      Location loc);

  public:
    /// Returns the name of the symbol.
    Symbol get_name() const { return m_sym; }

    /// Returns the location of the introduced name. This is associated
    /// with the point of identification.
    Location get_name_location() const { return m_loc; }

  private:
    /// The name of the declared entity.
    Symbol m_sym;

    /// The location of the name.
    Location m_loc;
  };

  inline
  Named_declaration::Named_declaration(Kind k,
                                       Scoped_declaration* sd, 
                                       Symbol sym, 
                                       Location start, 
                                       Location loc)
    : Declaration(k, sd, start), m_sym(sym), m_loc(loc)
  { }


  /// The base class of all declarations that have an associated type. Note
  /// that type specifiers are associated with the derived classes, not this
  /// class.
  class Typed_declaration : public Named_declaration
  {
  protected:
    Typed_declaration(Kind k, 
                      Scoped_declaration* sd, 
                      Symbol sym, 
                      Location start,
                      Location loc);

  public:
    /// Returns the type of the declaration.
    Type* get_type() const { return m_type; }

    /// Sets the type of the declaration. 
    void set_type(Type* t) { assert(!m_type); m_type = t; }

  private:
    Type* m_type;
  };

  inline 
  Typed_declaration::Typed_declaration(Kind k, 
                                       Scoped_declaration* sd, 
                                       Symbol sym, 
                                       Location start,
                                       Location loc)
    : Named_declaration(k, sd, sym, start, loc), m_type()
  { }


  /// Represents the declaration of a function.
  class Function_declaration : public Typed_declaration, public Scoped_declaration
  {
  public:
    Function_declaration(Scoped_declaration* sd, 
                         Symbol sym, 
                         Location start, 
                         Location loc);

    // Type

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

    // Parameters

    /// Returns the parameters.
    const Parameter_seq& get_parameters() const { return m_parms; }
    
    /// Returns the parameters.
    Parameter_seq& get_parameters() { return m_parms; }

    /// Sets the parameters.
    void set_parameters(Parameter_seq&& parms) { m_parms = std::move(parms); }

    // Return type specifier

    /// Returns the return type specifier.
    Type_specifier* get_return() const { return m_ret; }

    /// Sets the return type specifier.
    void set_return(Type_specifier* ts) { m_ret = ts; }

    // Function definition

    /// Returns the body of the statement.
    Statement* get_body() const { return m_body; }

    /// Sets the body of the statement.
    void set_body(Statement* s) { assert(!m_body); m_body = s; }

  private:
    /// The parameters of the function.
    Parameter_seq m_parms;

    /// The return type specifier.
    Type_specifier* m_ret;

    /// The statement defining the function.
    ///
    /// FIXME: The definition could also be deleted or defaulted.
    Statement* m_body;
  };

  inline
  Function_declaration::Function_declaration(Scoped_declaration* sd, 
                                             Symbol sym, 
                                             Location start, 
                                             Location loc)
    : Typed_declaration(func_kind, sd, sym, start, loc), 
      Scoped_declaration(this),
      m_parms(),
      m_ret(),
      m_body()
  { }


  /// The base class of all data declarations. This class provides an optional
  /// type specifier to indicate the declared type of the value or object.
  class Data_declaration : public Typed_declaration
  {
  protected:
    Data_declaration(Kind k, 
                     Scoped_declaration* sd, 
                     Symbol sym, 
                     Location start, 
                     Location loc);

  public:
    /// Returns the type specifier for the declaration.
    Type_specifier* get_type_specifier() const { return m_ts; }

    /// Sets the type specifier for the declaration.
    void set_type_specifier(Type_specifier* ts) { assert(!m_ts); m_ts = ts; }

    /// Returns the initializer.
    Expression* get_initializer() const { return m_init; }

    /// Sets the initializer.
    void set_initializer(Expression* e);

    // Storage class

    /// Returns true if the declaration has static storage. This is the case
    /// when the declaration is global.
    ///
    /// \todo Allow for local statics?
    bool has_static_storage() const;

    /// Returns true if the declaration has automatic storage. This is the
    /// case for local variables.
    bool has_automatic_storage() const;

  private:
    /// The type specifier of data declaration. This may be omitted.
    Type_specifier* m_ts;

    /// The initializer for the data declaration. For parameters,
    /// this is interpreted as the default argument.
    Expression* m_init;
  };

  inline
  Data_declaration::Data_declaration(Kind k, 
                                     Scoped_declaration* sd, 
                                     Symbol sym, 
                                     Location start, 
                                     Location loc)
    : Typed_declaration(k, sd, sym, start, loc), m_ts(), m_init()
  { }


  /// Represents the declaration of a value. Value declarations introduce 
  /// names that bind to expressions, which compute values.
  class Value_declaration : public Data_declaration
  {
  public:
    Value_declaration(Scoped_declaration* sd, 
                      Symbol sym, 
                      Location start, 
                      Location loc)
      : Data_declaration(val_kind, sd, sym, start, loc)
    { }
  };


  /// Represents the declaration of a variable. Variable declarations introduce
  /// names that bind to objects, which hold values. These declarations are
  /// responsible for the allocation of storage for the object.
  class Variable_declaration : public Data_declaration
  {
  public:
    Variable_declaration(Scoped_declaration* sd, 
                         Symbol sym, 
                         Location start,
                         Location loc)
      : Data_declaration(var_kind, sd, sym, start, loc)
    { }
  };


  /// Represents the declaration of a reference. Reference declarations 
  /// introduce names that bind to objects, which hold values. Unlike 
  /// variables, there may be no underlying storage for the object.
  class Reference_declaration : public Data_declaration
  {
  public:
    Reference_declaration(Scoped_declaration* sd, 
                          Symbol sym, 
                          Location start, 
                          Location loc)
      : Data_declaration(ref_kind, sd, sym, start, loc)
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

    // Position

    /// Returns the depth of the parameter.
    int get_depth() const { return m_depth; }
    
    /// Returns the index of the parameter.
    int get_index() const { return m_index; }

    /// Sets the depth and index of the parameter.
    void set_position(int d, int n) { m_depth = d; m_index = n; }

  private:
    /// The underlying data or type declaration.
    Named_declaration* m_decl;
    
    /// The 0-based depth of the parameter. This can be non-zero for nested
    /// templates and lambda expressions.
    int m_depth;
    
    /// The 0-based index of the parameter in its construct.
    int m_index;
  };

  inline Type*
  Parameter::get_type() const
  {
    assert(dynamic_cast<const Typed_declaration*>(m_decl));
    return static_cast<const Typed_declaration*>(m_decl)->get_type();
  }


  /// An assertion verifies (statically or dynamically) that a condition
  /// is true. Assertions outside of block scope are static; inside they are
  /// dynamic, unless that static specifier is given.
  class Assertion : public Declaration
  {
  public:
    Assertion(Scoped_declaration* sd, Expression* e, Location kw)
      : Declaration(assert_kind, sd, kw), m_expr(e)
    { }

    /// Returns the condition.
    Expression* get_condition() const { return m_expr; }

  private:
    Expression* m_expr;
  };

} // namespace beaker
