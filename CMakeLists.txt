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

include(GNUInstallDirs)

option(LIBBECH32_BUILD_GOOGLETEST OFF)
option(LIBBECH32_BUILD_RAPIDCHECK OFF)
option(INSTALL_LIBBECH32 "Enable installation of libbech32. (Projects embedding libbech32 may want to turn this OFF.)" ON)
add_subdirectory(libbech32)

# Set option to build tests or not. Other projects that embed libtxref
# may want to set this to OFF
set(LIBTXREF_BUILD_TESTS ON CACHE BOOL "LIBTXREF_BUILD_TESTS")

# Set option to build examples or not. Other projects that embed libtxref
# may want to set this to OFF
set(LIBTXREF_BUILD_EXAMPLES ON CACHE BOOL "LIBTXREF_BUILD_EXAMPLES")

# Set option to install libtxref library and header file. Other projects that embed libtxref may want to set this to OFF
option(INSTALL_LIBTXREF "Enable installation of libtxref. (Projects embedding libtxref may want to turn this OFF.)" ON)

add_subdirectory(libtxref)

if(LIBTXREF_BUILD_TESTS)
  enable_testing()
  # Set options so we build googletest and rapidcheck. Other projects that
  # embed libtxref should set these options to OFF
  option(LIBTXREF_BUILD_GOOGLETEST "Build googletest" ON)
  option(LIBTXREF_BUILD_RAPIDCHECK "Build rapidcheck" ON)
  add_subdirectory(test)
endif()

if(LIBTXREF_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
