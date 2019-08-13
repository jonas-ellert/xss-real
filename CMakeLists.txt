cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
project(nss CXX C)
set(CMAKE_POLICY_DEFAULT_CMP0048 NEW)

function(print TYPE TEXT)
    string(ASCII 27 Esc)
    message(${TYPE} "${Esc}[1m${Esc}[36m[XSS]${Esc}[m ${Esc}[1m${TEXT}${Esc}[m")
endfunction()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

# Default to Release build
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()
print(STATUS "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")

# Turn on malloc count by default
option(ENABLE_MALLOC_COUNT "Use malloc_count to measure memory usage." ON)
print(STATUS "ENABLE_MALLOC_COUNT=${ENABLE_MALLOC_COUNT}")


# REQUIRE G++/GCC 7.3
# Require G++ version
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS "7.3")
        print(FATAL_ERROR "g++ version 7.3 or greater required!")
    endif()
else()
    print(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER_ID} is not supported!")
endif()
print(STATUS "g++ version: ${CMAKE_CXX_COMPILER_VERSION}")

# Check GCC version
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    if(${CMAKE_C_COMPILER_VERSION} VERSION_LESS "7.3")
        print(FATAL_ERROR "gcc version 7.3 or greater required!")
    endif()
else()
    print(FATAL_ERROR "Compiler ${CMAKE_C_COMPILER_ID} is not supported!")
endif()
print(STATUS "gcc version: ${CMAKE_C_COMPILER_VERSION}")

# Compiler flags         -Winline \ -Wno-error=inline \
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
        -fopenmp \
        -Wall \
        -Wextra \
        -Wpedantic \
        -fdiagnostics-color=auto \
        --param large-function-growth=10000 \
        --param inline-unit-growth=1000 \
        -fconstexpr-loop-limit=4194304")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} \
        -O3 \
        -ffast-math \
        -funroll-loops \
        -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} \
        -O0 \
        -ggdb")

# Initialize external dependencies
print(STATUS "Updating git submodules...")
execute_process(COMMAND git submodule update --init)

# Count CPUs
include(ProcessorCount)
ProcessorCount(CORES)
if(CORES EQUAL 0)
    set(CORES 1)
endif()

# Build SDSL
print(STATUS "Building sdsl-lite...")
execute_process(COMMAND cmake ${CMAKE_CURRENT_SOURCE_DIR}/external/sdsl-lite
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build-sdsl-lite)
execute_process(COMMAND make -j${CORES}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build-sdsl-lite)
print(STATUS "Building sdsl-lite: Done.")

# SDSL libraries
print(STATUS "Adding sdsl (version 1.0) includes...")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/sdsl/include)
print(STATUS "Adding sdsl-lite (version 2.0) libraries and includes...")
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/build-sdsl-lite/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/build-sdsl-lite/include)
# divsufsort libraries
print(STATUS "Adding divsufsort libraries and includes (import from sdsl-lite)...")
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/build-sdsl-lite/external/libdivsufsort/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/build-sdsl-lite/external/libdivsufsort/include)
# gtest libraries
print(STATUS "Adding gtest libraries and includes (import from sdsl-lite)...")
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/build-sdsl-lite/external/googletest/googletest)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/sdsl-lite/external/googletest/googletest/include)

# Add TLX command line parser
print(STATUS "Adding tlx_command_line libraries and includes...")
add_library(tlx_command_line
        external/tlx/tlx/cmdline_parser.cpp
        external/tlx/tlx/string/parse_si_iec_units.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/tlx)

# Add nss-real
print(STATUS "Adding nss-real includes...")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/nss-real/include)

# Add GSACA
print(STATUS "Adding gsaca libraries and includes...")
add_library(gsaca external/gsaca-lyndon/gsaca.c)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/gsaca-lyndon)

# Add Prezza LCE
print(STATUS "Adding Prezza LCE includes...")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/prezza-lce/include)

# Add Prezza LCE
print(STATUS "Adding Prezza LCE includes...")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/rk-lce/internal)

if(ENABLE_MALLOC_COUNT)
    # Add GSACA
    print(STATUS "Adding malloc_count libraries and includes...")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi")
    add_library(malloc_count external/malloc_count/malloc_count.c)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/malloc_count)
    add_definitions(-DMALLOC_COUNT)
endif(ENABLE_MALLOC_COUNT)

print(STATUS "Adding XSS includes...")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

print(STATUS "List of include directories:")
get_property(all_include_dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${all_include_dirs})
    print(STATUS "    ${dir}")
endforeach()

add_subdirectory(test)
add_subdirectory(src)

MESSAGE(STATUS "Built Type: " ${CMAKE_BUILD_TYPE} )