cmake_minimum_required(VERSION 3.15)
project(LivingObjects2 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/tests)

# Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# VM core library (real implementations)
add_library(vm_core
    src/tagged_value.cpp
    src/classes/compiled_method.cpp
    src/classes/byte_array.cpp
    src/classes/array.cpp
)

# Test helpers library
add_library(bytecode_test_helpers
    tests/bytecode/bytecode_test_helpers.cpp
    src/classes/context.cpp
)
target_include_directories(bytecode_test_helpers PUBLIC
    ${CMAKE_SOURCE_DIR}/tests/bytecode
)
target_link_libraries(bytecode_test_helpers
    vm_core
)

# Bytecode instruction tests
add_executable(bytecode_instructions_test
    tests/bytecode/bytecode_instructions_test.cpp
)
target_link_libraries(bytecode_instructions_test
    bytecode_test_helpers
    GTest::gtest
    GTest::gtest_main
)

# TaggedValue unit tests
add_executable(tagged_value_test
    tests/unit/tagged_value_test.cpp
)
target_link_libraries(tagged_value_test
    vm_core
    GTest::gtest
    GTest::gtest_main
)

# Enable testing
enable_testing()
add_test(NAME BytecodeInstructionsTest COMMAND bytecode_instructions_test)
add_test(NAME TaggedValueTest COMMAND tagged_value_test)

# Print configuration
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")

