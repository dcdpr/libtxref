cmake_minimum_required(VERSION 3.8)

project(libtxref VERSION 1.0.0
                 DESCRIPTION "Digital Contract Design's C++ implementation library for creating and decoding TXREFs (BIP-0136)"
                 LANGUAGES C CXX )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
  set(DCD_CXX_FLAGS -Wall -Wextra -Wpedantic -Wno-unknown-pragmas)
elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang)
  set(DCD_CXX_FLAGS -Weverything -Wno-c++98-compat -Wno-padded -Wno-missing-prototypes)
endif()

option(LIBBECH32_BUILD_GOOGLETEST OFF)
option(LIBBECH32_BUILD_RAPIDCHECK OFF)
add_subdirectory(libbech32)

add_subdirectory(libtxref)

enable_testing()

# Set options so we build googletest and rapidcheck. Other projects that
# embed libtxref should set these options to OFF
option(LIBTXREF_BUILD_GOOGLETEST "Build googletest" ON)
option(LIBTXREF_BUILD_RAPIDCHECK "Build rapidcheck" ON)
add_subdirectory(test)

add_subdirectory(examples)
