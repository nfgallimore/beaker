
add_library(beaker.lang
  context.cpp
  symbol.cpp

  file.cpp
  location.cpp
  token.cpp

  type.cpp
  type_specifier.cpp
  expression.cpp
  arithmetic_expression.cpp
  logical_expression.cpp
  conversion.cpp
  statement.cpp
  declaration.cpp

  lexer.cpp
  parser.cpp
  module_parser.cpp
  type_parser.cpp
  expression_parser.cpp
  data_parser.cpp
  function_parser.cpp

  semantics.cpp
  expression_semantics.cpp
  conversion_semantics.cpp
  scope.cpp
  print.cpp
  dump.cpp)

add_executable(beaker.compile main.cpp)
target_link_libraries(beaker.compile beaker.lang)
