#pragma once

namespace beaker {

/// Represents a location in an input. This stores an offset into a text
/// buffer. We represent locations in two ways: a simple line/column format
/// that can be encoded directly, and an index into a table of files and
/// text buffers. The first is intended to REPL usage where there are no
/// input files. The latter is used to support offline compilation, where a
/// list text buffers is being translated as source code.
///
/// \todo How do we represent source code locations in imported modules? We're
/// not processing their text buffers in the same way. These could simply be
/// keys into a location table that stores locations. Note that this is going
/// to get complicated when we have templates being instantiated from imported
/// modules (the source location table gets big!).
class Location
{
public:
  /// Constructs a default (invalid) offset.
  Location()
    : m_offset(-1)
  { }

  /// Constructs an offset. 
  explicit Location(unsigned off)
    : m_offset(off)
  { }

  /// Converts to true if the offset is valid.
  explicit operator bool() const 
  {
    return m_offset != -1;
  }

private:
  unsigned m_offset;
};

} // namespace beaker
