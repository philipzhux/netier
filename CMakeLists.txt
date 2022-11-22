cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # For clang-tidy.
set(BUILD_SHARED_LIBS ON)
set(CMAKE_CXX_STANDARD 17)              # Compile as C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)     # Require C++17 support

project(Netier
        VERSION 1.0
        DESCRIPTION "Event-based concurrent TCP server framework with reactor pattern."
        LANGUAGES C CXX
        )

# People keep running CMake in the wrong folder, completely nuking their project or creating weird bugs.
# This checks if you're running CMake from a folder that already has CMakeLists.txt.
# Importantly, this catches the common case of running it from the root directory.
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" PATH_TO_CMAKELISTS_TXT)
if (EXISTS "${PATH_TO_CMAKELISTS_TXT}")
    message(FATAL_ERROR "Run CMake from a build subdirectory! \"mkdir build ; cd build ; cmake ..\" \
    Some junk files were created in this folder (CMakeCache.txt, CMakeFiles); you should delete those.")
endif ()

# Expected directory structure.
set(NETIER_BUILD_SUPPORT_DIR "${CMAKE_SOURCE_DIR}/build_support")
set(NETIER_CLANG_SEARCH_PATH "/usr/local/bin" "/usr/bin" "/usr/local/opt/llvm/bin" "/usr/local/opt/llvm@8/bin"
        "/usr/local/Cellar/llvm/8.0.1/bin")

######################################################################################################################
# DEPENDENCIES
######################################################################################################################

# CTest
# enable_testing()

# clang-format
find_program(CLANG_FORMAT_BIN
        NAMES clang-format clang-format-8
        HINTS ${NETIER_CLANG_SEARCH_PATH})
if ("${CLANG_FORMAT_BIN}" STREQUAL "CLANG_FORMAT_BIN-NOTFOUND")
    message(WARNING "Netier/main couldn't find clang-format.")
else()
    message(STATUS "Netier/main found clang-format at ${CLANG_FORMAT_BIN}")
endif()

# clang-tidy
find_program(CLANG_TIDY_BIN
        NAMES clang-tidy clang-tidy-8
        HINTS ${NETIER_CLANG_SEARCH_PATH})
if ("${CLANG_TIDY_BIN}" STREQUAL "CLANG_TIDY_BIN-NOTFOUND")
    message(WARNING "Netier/main couldn't find clang-tidy.")
else()
    # Output compile_commands.json
    set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
    message(STATUS "Netier/main found clang-tidy at ${CLANG_TIDY_BIN}")
endif()

# cpplint
find_program(CPPLINT_BIN
        NAMES cpplint cpplint.py
        HINTS "${NETIER_BUILD_SUPPORT_DIR}")
if ("${CPPLINT_BIN}" STREQUAL "CPPLINT_BIN-NOTFOUND")
    message(WARNING "Netier/main couldn't find cpplint.")
else()
    message(STATUS "Netier/main found cpplint at ${CPPLINT_BIN}")
endif()

# # gmock gtest https://crascit.com/2015/07/25/cmake-gexamples/
# configure_file("${PROJECT_SOURCE_DIR}/build_support/gtest_CMakeLists.txt.in" googletest-download/CMakeLists.txt)
# execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
#         WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download")
# execute_process(COMMAND "${CMAKE_COMMAND}" --build .
#         WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download")
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)  # don't override our compiler/linker options when building gtest
# add_subdirectory("${CMAKE_BINARY_DIR}/googletest-src" "${CMAKE_BINARY_DIR}/googletest-build")

######################################################################################################################
# COMPILER SETUP
######################################################################################################################

# Compiler flags.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wextra -Werror -Wno-reorder -std=c++17 -pthread")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-attributes") #TODO: remove
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fPIC")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fPIC")
set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -fPIC")

set(GCC_COVERAGE_LINK_FLAGS    "-fPIC")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")

# Output directory.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Includes.
set(NETIER_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(NETIER_EXAMPLE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/examples/include ${PROJECT_SOURCE_DIR}/examples/)
set(NETIER_THIRD_PARTY_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/third_party)
include_directories(${NETIER_SRC_INCLUDE_DIR} ${NETIER_EXAMPLE_INCLUDE_DIR} ${NETIER_THIRD_PARTY_INCLUDE_DIR})
# include_directories(BEFORE src) # This is needed for gtest.

add_subdirectory(src)
add_subdirectory(examples)
######################################################################################################################
# MAKE TARGETS
######################################################################################################################

##########################################
# "make format"
# "make check-format"
##########################################

string(CONCAT NETIER_FORMAT_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/src,"
        "${CMAKE_CURRENT_SOURCE_DIR}/examples,"
        )

# runs clang format and updates files in place.
add_custom_target(format ${NETIER_BUILD_SUPPORT_DIR}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${NETIER_BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
        --source_dirs
        ${NETIER_FORMAT_DIRS}
        --fix
        --quiet
        )

# runs clang format and exits with a non-zero exit code if any files need to be reformatted
add_custom_target(check-format ${NETIER_BUILD_SUPPORT_DIR}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${NETIER_BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
        --source_dirs
        ${NETIER_FORMAT_DIRS}
        --quiet
        )


##########################################
# "make check-lint"
##########################################

file(GLOB_RECURSE NETIER_LINT_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.cpp"
        )

# Balancing act: cpplint.py takes a non-trivial time to launch,
# so process 12 files per invocation, while still ensuring parallelism
add_custom_target(check-lint echo '${NETIER_LINT_FILES}' | xargs -n12 -P8
        ${CPPLINT_BIN}
        --verbose=2 --quiet
        --linelength=120
        --filter=-legal/copyright,-build/header_guard
        )

###########################################################
# "make check-clang-tidy" target
###########################################################
# runs clang-tidy and exits with a non-zero exit code if any errors are found.
# note that clang-tidy automatically looks for a .clang-tidy file in parent directories
add_custom_target(check-clang-tidy
        ${NETIER_BUILD_SUPPORT_DIR}/run_clang_tidy.py                     # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN}                              # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR}                                            # using cmake's generated compile commands
        )
add_dependencies(check-clang-tidy netier_shared)                    