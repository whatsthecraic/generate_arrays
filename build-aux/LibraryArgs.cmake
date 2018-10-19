# Find the library `args'. If not present, download & install in lib/args.

include_guard(GLOBAL)

find_path(ARGS_INCLUDE_DIRS NAMES args.hxx)

if(NOT ${ARGS_INCLUDE_DIRS})
    # Download & build libbacktrace
    include(ExternalProject)
    ExternalProject_Add(args_external
        GIT_REPOSITORY "https://github.com/Taywee/args" # upstream repository
        GIT_TAG "6.2.0" # version of the library
        SOURCE_DIR "${PROJECT_SOURCE_DIR}/lib/args" # where to download the library
        PREFIX "${CMAKE_BINARY_DIR}/lib/args/cmake" # cmake logs and other rubbish
        BUILD_IN_SOURCE YES # this is a header only library
        INSTALL_DIR "${CMAKE_BINARY_DIR}/lib/args"
        CONFIGURE_COMMAND "" # configure step, nop, there is nothing to configure
        BUILD_COMMAND ${CMAKE_COMMAND} -E make_directory <INSTALL_DIR> # just mkdir -pv the final directory where to move the headers
        INSTALL_COMMAND make install DESTDIR=<INSTALL_DIR> # move the final headers
        BUILD_BYPRODUCTS "${CMAKE_BINARY_DIR}/lib/args/include/args.hxx"
        UPDATE_COMMAND "" # hack, avoid to rebuild the whole library once built
        PATCH_COMMAND ""
    )

    # Create the target
    add_library(args INTERFACE IMPORTED)
    add_dependencies(args args_external)
    # ensure that the include directory exists even before the libbacktrace is created
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/lib/args/include")
    set_target_properties(args PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/lib/args/include")
else()
    add_library(args INTERFACE IMPORTED)
    set_target_properties(args PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${ARGS_INCLUDE_DIRS}")
endif()