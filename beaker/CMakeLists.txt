
add_library(beaker.lang
  hash.cpp
  factory.cpp
  symbol.cpp

  file.cpp
  location.cpp
  token.cpp

  type.cpp
  type_specifier.cpp
  expression.cpp
  arithmetic_expression.cpp
  bitwise_expression.cpp
  logical_expression.cpp
  relational_expression.cpp
  conversion.cpp
  initializer.cpp
  statement.cpp
  declaration.cpp
  context.cpp

  lexer.cpp
  parser.cpp
  module_parser.cpp
  type_parser.cpp
  expression_parser.cpp
  statement_parser.cpp
  data_parser.cpp
  function_parser.cpp

  semantics.cpp
  type_semantics.cpp
  expression_semantics.cpp
  conversion_semantics.cpp
  statement_semantics.cpp
  declaration_semantics.cpp
  initialization_semantics.cpp
  scope.cpp
  print.cpp
  dump.cpp

  evaluation.cpp
  expression_evaluation.cpp
  statement_evaluation.cpp
  declaration_evaluation.cpp
  value.cpp
  object.cpp

  generation.cpp
  global_generation.cpp
  module_generation.cpp
  variable_generation.cpp
  function_generation.cpp
  instruction_generation.cpp)
target_link_libraries(beaker.lang )

add_executable(beaker.compile main.cpp)
target_link_libraries(beaker.compile beaker.lang ${LLVM_LIBS})
