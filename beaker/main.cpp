#include <beaker/context.hpp>
#include <beaker/file.hpp>
#include <beaker/module_parser.hpp>

#include <iostream>

using namespace beaker;

int 
main(int argc, const char* argv[])
{
  if (argc < 2) {
    std::cerr << "usage: beaker-compile <input-files>\n";
    return 1;
  }
  
  // The global translation context.
  Context cxt;
  
  // The input file.
  File input(argv[1]);

  // Run the parser.
  Parse_context pcxt(cxt, input);
  Module_parser mp(pcxt);
  Declaration* module = mp.parse_module();
}
