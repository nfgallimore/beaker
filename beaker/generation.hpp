#pragma once

#include <beaker/common.hpp>

namespace beaker
{
  /// Maintains essential state for a code generation.
  ///
  /// \todo Support multiple code generation facilities (e.g., for GCC?).
  class Generator
  {
  public:
    Generator(Context& cxt);
    ~Generator();

    /// Generate the IR code for this translation unit.
    void generate_module(const Declaration* tu);

  private:
    class Generation_context;

    /// The global translation facility.
    std::unique_ptr<Generation_context> m_cxt;
  };

} // namespace beaker
