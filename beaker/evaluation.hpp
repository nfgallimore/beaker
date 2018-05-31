#pragma once

#include <beaker/common.hpp>
#include <beaker/value.hpp>
#include <beaker/object.hpp>

#include <forward_list>
#include <unordered_map>

#include <boost/optional.hpp> // FIXME: Remove this.

namespace beaker
{
  class Typed_declaration;

  /// The static store provides facilities for managing static name bindings.
  /// This associates declarations with their corresponding values. In cases
  /// where those declarations have associated objects, this will also manage
  /// the storage for those objects.
  class Static_store
  {
    using Object_list = std::forward_list<Object>;
    using Value_map = std::unordered_map<const Typed_declaration*, Value>;
  public:
    /// Associates a declaration directly with a value. This must only be
    /// called directly for constants.
    void bind(const Typed_declaration* d, const Value& v);

    /// Creates an object for the given declaration. Note that the object is 
    /// uninitialized at the time of creation. The evaluator is responsible
    /// for populating the object with an initial value.
    Object* create(const Typed_declaration* d);

    /// Fetches the value associated with the declaration. If the declaration
    /// has an associated object
    Value get(const Typed_declaration* d);

    /// Returns a possibly constructed value if a binding exists.
    boost::optional<Value> get_if(const Typed_declaration* d);

  private:
    void do_bind(const Typed_declaration* d, const Value& v);

  private:
    /// Associates declarations with objects.
    Object_list m_objects;

    /// Associates declarations with values.
    Value_map m_values;
  };


  /// Provides context for the evaluation of expressions and statements.
  ///
  /// \todo Would it be useful to record the effects of evaluation for
  /// subsequent analysis? Might be helpful for e.g., validating contracts
  /// that involve side effects.
  ///
  /// \todo We need to provide evaluation modes so that we can make best
  /// effort evaluations (e.g., during code generation) or require evaluations
  /// (e.g., when processing a constant expression). We might also provide
  /// limited forms of evaluation like constant folding, which would not
  /// invoke function calls.
  class Evaluator
  {
  public:
    enum Mode 
    {
      /// A simple form of evaluation that folds constant expressions.
      /// This limits 
      constant_fold,

      /// Applies the rules of constant expression evaluation and fails if
      /// any non-constant expressions are evaluated or undefined behaviors
      /// are encountered.
      constant_eval,

      /// Applies the rules of constant expression evaluation, except that
      /// it is not an error to encounter non-constant expressions.
      potential_eval,
    };

    Evaluator(Context& cxt, Mode mode)
      : m_cxt(cxt)
    { }

    /// Evaluate an expression, returning a value.
    Value evaluate(const Expression* e);

    // Basic expressions
    Value evaluate_bool_literal(const Bool_literal* e);
    Value evaluate_int_literal(const Int_literal* e);
    Value evaluate_id_expression(const Id_expression* e);
    Value evaluate_init_expression(const Id_expression* e);

    // Logical expressions
    Value evaluate_conditional_expression(const Conditional_expression* e);
    Value evaluate_logical_and_expression(const Logical_and_expression* e);
    Value evaluate_logical_or_expression(const Logical_or_expression* e);
    Value evaluate_logical_not_expression(const Logical_not_expression* e);
  
    // Conversions
    Value evaluate_implicit_conversion(const Implicit_conversion* e);
    Value convert_to_value(const Expression* e);

    // Initializers
    Value evaluate_value_initializer(const Value_initializer* e);

    // Declarations

    /// Elaborates a declaration, possibly creating storage. This evaluates
    /// the initializer so that, after elaboration, `d` is bound to a value.
    ///
    /// In cases where `d` is non-typed, this applies the effect of the
    /// declaration (often none).
    void elaborate(const Declaration* d);
    void elaborate_function(const Function_declaration* d);
    void elaborate_variable(const Variable_declaration* d);
    void elaborate_constant(const Data_declaration* d);

    // Generalized store

    /// Fetch the value of a declaration.
    Value fetch(const Typed_declaration* d);
    Value fetch_data(const Data_declaration* d);
    Value fetch_static(const Typed_declaration* d);
    Value fetch_automatic(const Data_declaration* d);
    Value fetch_function(const Function_declaration* d);

  private:
    /// The translation context.
    Context& m_cxt;

    /// The static store.
    Static_store m_statics;
  };


  /// A helper class that constructs an evaluator for constant folding.
  class Constant_folder : public Evaluator
  {
  public:
    Constant_folder(Context& cxt)
      : Evaluator(cxt, constant_fold)
    { }
  };


  /// A helper class that constructs an evaluator for constant expression
  /// evaluation.
  class Constant_evaluator : public Evaluator
  {
  public:
    Constant_evaluator(Context& cxt)
      : Evaluator(cxt, constant_eval)
    { }
  };


  // ------------------------------------------------------------------------ //
  // Helper functions

  inline Value 
  evaluate(Context& cxt, const Expression* e)
  {
    Constant_evaluator eval(cxt);
    return eval.evaluate(e);
  }


} // namespace beaker
