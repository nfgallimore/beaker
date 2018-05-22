
cmake_minimum_required(VERSION 3.9)

project(beaker CXX)

find_package(LLVM REQUIRED CONFIG)

# # Re-run llvm-config to get linker flags. When multiple versions of LLVM are
# # available, we need to make sure that we link against the right ones. By
# # default, the linker will try to find the system version even though we're
# # finding a different configuration.
# execute_process(
#   COMMAND ${LLVM_TOOLS_BINARY_DIR}/llvm-config --ldflags
#   OUTPUT_VARIABLE LLVM_LINK_FLAGS)

# message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
# message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
# message(STATUS "Using LLVM tools in: ${LLVM_TOOLS_BINARY_DIR}")
# message(STATUS "Using LLVM link flags: ${LLVM_LINK_FLAGS}")

# Generate library names for LLVM libraries.
llvm_map_components_to_libnames(LLVM_LIBS core native)

# Make sure the headers will be available to #include.
include_directories(${LLVM_INCLUDE_DIRS})

# Specify additional macro definitions.
add_definitions(${LLVM_DEFINITIONS})

set(CMAKE_CXX_FLAGS "-std=c++17")

include_directories(.)

add_subdirectory(beaker)
