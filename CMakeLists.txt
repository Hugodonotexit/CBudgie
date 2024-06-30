# Specify the minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Define the project name and version
project(InterpreterProject VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Ofast -fopenmp")

# Include directories
include_directories(depen lexer parser)

# Source files
file(GLOB SRCS
    "${CMAKE_SOURCE_DIR}/interpreter/*.cpp"
    "${CMAKE_SOURCE_DIR}/interpreter/lexer/*.cpp"
    "${CMAKE_SOURCE_DIR}/interpreter/parser/*.cpp"
)

# Check if SRCS variable is empty
if (SRCS STREQUAL "")
    message(FATAL_ERROR "No source files found. Please check your file paths.")
endif()

# Create build directory
set(BUILDDIR ${CMAKE_BINARY_DIR})
file(MAKE_DIRECTORY ${BUILDDIR})

# Add executable target
add_executable(cbudgie ${SRCS})

# Set output directory for the executable
set_target_properties(cbudgie PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BUILDDIR}
)

# Compile options (similar to CXXFLAGS)
target_compile_options(cbudgie PRIVATE -march=native -Ofast -fopenmp)

# Include directories (similar to CXXFLAGS -I)
target_include_directories(cbudgie PRIVATE ${CMAKE_SOURCE_DIR}/interpreter/depen ${CMAKE_SOURCE_DIR}/interpreter/lexer ${CMAKE_SOURCE_DIR}/interpreter/parser)

# Installation rules
install(TARGETS cbudgie
    RUNTIME DESTINATION bin
)