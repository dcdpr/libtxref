#
# Possible Input Settings (defined in environment or set on command line)
#
# LIBTXREF_PROJECT : Project name. Default: "libtxref"
#
# LIBTXREF_BUILD_TESTS : Build test executables [ON OFF]. Default: ON.
# LIBTXREF_BUILD_EXAMPLES : Build example executables [ON OFF]. Default: ON.
# INSTALL_LIBTXREF : Enable installation [ON OFF]. Default: ON.
#
# The test executables use googletest and rapidcheck. Other projects that
# embed libtxref and already use googletest and/or rapidcheck could set
# these options to OFF
# LIBTXREF_BUILD_GOOGLETEST : Build googletest for testing [ON OFF]. Default: ON if LIBTXREF_BUILD_TESTS is ON.
# LIBTXREF_BUILD_RAPIDCHECK : Build rapidcheck for testing [ON OFF]. Default: ON if LIBTXREF_BUILD_TESTS is ON.

cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# Version settings

set(LIBTXREF_VERSION_MAJOR 1)
set(LIBTXREF_VERSION_MINOR 0)
set(LIBTXREF_VERSION_PATCH 1)
set(LIBTXREF_VERSION ${LIBTXREF_VERSION_MAJOR}.${LIBTXREF_VERSION_MINOR}.${LIBTXREF_VERSION_PATCH} CACHE STRING "" FORCE)

# General settings

set(LIBTXREF_PROJECT "libtxref" CACHE STRING "Project Name")

# Test settings

set(LIBTXREF_BUILD_TESTS ON CACHE BOOL "Build test executables")

# Examples settings

set(LIBTXREF_BUILD_EXAMPLES ON CACHE BOOL "Build example executables")

# Install

set(INSTALL_LIBTXREF ON CACHE BOOL "Enable installation")

# Set default install prefix if we are top-level project, but only
# if it hasn't been set on the command line
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  if(NOT WIN32 AND CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(CMAKE_INSTALL_PREFIX "/opt/contract.design/libtxref" CACHE PATH "" FORCE)
  endif()
endif()

# Project

project(${LIBTXREF_PROJECT} VERSION ${LIBTXREF_VERSION}
                 DESCRIPTION "Digital Contract Design's C++ implementation library for creating and decoding TXREFs (BIP-0136)"
                 LANGUAGES C CXX )

# Log settings

message(STATUS "---------------- LIBTXREF OPTIONS ----------------")

message(STATUS "LIBTXREF_PROJECT            : " ${LIBTXREF_PROJECT})
message(STATUS "LIBTXREF_VERSION            : " ${LIBTXREF_VERSION})

message(STATUS "LIBTXREF_BUILD_TESTS        : " ${LIBTXREF_BUILD_TESTS})
message(STATUS "LIBTXREF_BUILD_EXAMPLES     : " ${LIBTXREF_BUILD_EXAMPLES})

message(STATUS "INSTALL_LIBTXREF            : " ${INSTALL_LIBTXREF})

message(STATUS "--------------------------------------------------")


include(GNUInstallDirs)

# Add libbech32 project from subdirectory
option(LIBBECH32_BUILD_GOOGLETEST OFF)
option(LIBBECH32_BUILD_RAPIDCHECK OFF)
option(INSTALL_LIBBECH32 "Enable installation" ON)
add_subdirectory(libbech32)


add_subdirectory(libtxref)

if(LIBTXREF_BUILD_TESTS)
  enable_testing()
  # Set options to build googletest and rapidcheck or not. Other
  # projects that embed libtxref and already use googletest and/or
  # rapidcheck could set these options to OFF
  option(LIBTXREF_BUILD_GOOGLETEST "Build googletest" ON)
  option(LIBTXREF_BUILD_RAPIDCHECK "Build rapidcheck" ON)
  add_subdirectory(test)
endif()

if(LIBTXREF_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
