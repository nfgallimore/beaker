#include <beaker/context.hpp>
#include <beaker/file.hpp>
#include <beaker/module_parser.hpp>
#include <beaker/declaration.hpp>
#include <beaker/generation.hpp>

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
  //
  // FIXME: Register the input file with the context.
  File input(argv[1]);

  // Run the parser.
  //
  // FIXME: Can we make this a single declaration? Probably not because of
  // the sharing.
  Parse_context pc(cxt, input);
  Module_parser mp(pc);
  Declaration* tu = mp.parse_module();
  // tu->dump();

  Generator gen(cxt);
  gen.generate_module(tu);
}
