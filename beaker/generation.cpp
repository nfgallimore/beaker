#include "generation.hpp"
#include "global_generation.hpp"
#include "module_generation.hpp"
#include "declaration.hpp"

namespace beaker
{
  /// A clever trick to use private classes in a private implementation.
  class Generator::Generation_context : public Global_context
  {
  public:
    using Global_context::Global_context;
  };

  Generator::Generator(Context& cxt)
    : m_cxt(new Generation_context(cxt))
  { }

  Generator::~Generator()
  { }

  void
  Generator::generate_module(const Declaration* tu)
  {
    assert(tu->is_translation_unit());
    Module_context mod(*m_cxt, static_cast<const Translation_unit*>(tu));
    mod.generate();
  }

} // namespace beaker
