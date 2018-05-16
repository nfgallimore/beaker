#pragma once

#include <string>

namespace beaker
{

/// Represents a source file.
///
/// \todo Factor the text buffer into a separate facility so that we can
/// support input from the command line or from generated code.
///
/// On second thought, maybe that doesn't work the way I want. If we 
/// continually add buffers, then the REPL is going to be fairly leaky. You
/// couldn't really track locations cumulatively. Maybe we should offer two
/// encodings: one that's source from a file and one that simply encodes
/// line and offset (i.e., for the REPL).
class File
{
public:
  /// Construct a source file from the file indicated by `path`. This reads
  /// the contents of the file.
  File(const std::string& path);
  
  File(const std::string& path, unsigned base);

  /// Returns the path to the file.
  const std::string& get_path() const { return m_path; }
  
  /// Returns the text of the file.
  const std::string& get_text() const { return m_text; }

  /// Returns the base offset of the file.
  unsigned get_base_offset() const { return m_base; }

private:
  /// The path to the file.
  std::string m_path;

  /// The text of the file.
  std::string m_text;

  /// The base, starting offset of the file's text buffer.
  unsigned m_base;
};

} // namespace beaker
