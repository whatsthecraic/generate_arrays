cmake_minimum_required(VERSION 3.12)
set(executable "generate_array")

# Be verbose
set(CMAKE_VERBOSE_MAKEFILE ON)

# Prefer Clang over GCC
set(CMAKE_C_COMPILER_NAMES clang gcc icc cc)
set(CMAKE_CXX_COMPILER_NAMES clang++ g++ icpc c++ cxx)

project(${executable})
set(CMAKE_CXX_STANDARD 17)


# Search for the project modules in /build-aux/
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/build-aux/")

# Helpers
include(AppendCompilerFlag)
include(GetCompilerArgs)
include(LibraryArgs)
include(Trim)

# Emit full debug info in clang
get_c_compiler_flags("" c_flags)
get_cxx_compiler_flags("" cxx_flags)
if("${c_flags}" MATCHES "( |^)-g( |$)")
    append_compiler_flag("-fno-omit-frame-pointer" "C")
    append_compiler_flag("-fno-limit-debug-info" "C")
endif()
if("${cxx_flags}" MATCHES "( |^)-g( |$)")
    append_compiler_flag("-fno-omit-frame-pointer" "CXX")
    append_compiler_flag("-fno-limit-debug-info" "CXX")
endif()

# Create the list of objects
add_subdirectory(lib/common)
#add_subdirectory(src obj)

# Create the executable
add_executable(${executable} main.cpp)
target_link_libraries(${executable} PRIVATE args PUBLIC libcommon)

# Testsfile
#add_subdirectory(test)

get_c_compiler_flags(${executable} c_flags)
get_cxx_compiler_flags(${executable} cxx_flags)
message("Compiler C..........: ${CMAKE_C_COMPILER} ${c_flags}")
message("Compiler C++........: ${CMAKE_CXX_COMPILER} ${cxx_flags}")
