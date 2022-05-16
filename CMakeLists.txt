# This file is an adaptaiton of the original adopted from 3D Slicer https://github.com/Slicer/Slicer
#
cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME "DistanceMapsEvaluation")

#-----------------------------------------------------------------------------
# Enable C++11
#-----------------------------------------------------------------------------
set(_msg "Setting C++ standard")
message(STATUS "${_msg}")
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
message(STATUS "${_msg} - C++${CMAKE_CXX_STANDARD}")
if(NOT CMAKE_CXX_STANDARD MATCHES "^(14|17)$")
  message(FATAL_ERROR "CMAKE_CXX_STANDARD must be set to 11, 14 or 17")
endif()

#-----------------------------------------------------------------------------
# Add our CMake modules
#-----------------------------------------------------------------------------
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})
set(SUPERBUILD_TOPLEVEL_PROJECT ${PROJECT_NAME})
list(APPEND EXTERNAL_PROJECT_ADDITIONAL_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild)
include(ExternalProject)
include(ExternalProjectDependency)

#-----------------------------------------------------------------------------
# Pre-requisites
#-----------------------------------------------------------------------------
find_package(Git REQUIRED)
if(NOT GIT_FOUND)
  message(FATAL_ERROR "error: Install Git and try to re-configure")
endif()
mark_as_superbuild(GIT_EXECUTABLE)

#-----------------------------------------------------------------------------
# Set project and version -- HERE BEGINS THE PROJECT DEFINITION
#-----------------------------------------------------------------------------
if(NOT DEFINED ${PROJECT_NAME}_VERSION_MAJOR)
  set(${PROJECT_NAME}_VERSION_MAJOR "0")
endif()
if(NOT DEFINED ${PROJECT_NAME}_VERSION_MINOR)
  set(${PROJECT_NAME}_VERSION_MINOR "5")
endif()
if(NOT DEFINED ${PROJECT_NAME}_VERSION_PATCH)
  set(${PROJECT_NAME}_VERSION_PATCH "0")
endif()
project(${PROJECT_NAME} VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}")

#-----------------------------------------------------------------------------
# Superbuild Option - Enabled by default
#-----------------------------------------------------------------------------
option(${PROJECT_NAME}_SUPERBUILD "Build ${PROJECT_NAME} and the projects it depends on." ON)
mark_as_advanced(${PROJECT_NAME}_SUPERBUILD)
set(${PROJECT_NAME}_BINARY_INNER_SUBDIR ${PROJECT_NAME}-build)

#-----------------------------------------------------------------------------
# Sanity checks
#------------------------------------------------------------------------------
if(WIN32)
  set(${PROJECT_NAME}_ROOT_DIR_MAX_LENGTH 40)
  if(NOT ${PROJECT_NAME}_SUPERBUILD)
    string(LENGTH ${${PROJECT_NAME}_BINARY_INNER_SUBDIR} _inner_subdir_length)
    math(EXPR ${PROJECT_NAME}_ROOT_DIR_MAX_LENGTH "${${PROJECT_NAME}_ROOT_DIR_MAX_LENGTH} + ${_inner_subdir_length}")
  endif()
  include(PreventDirWithTooManyChars)
endif()
include(PreventInSourceBuilds)
include(PreventInBuildInstalls)
include(PreventDirWithSpaces)
mark_as_superbuild(${PROJECT_NAME}_SKIP_DIR_WITH_SPACES_CHECK:BOOL)

#-----------------------------------------------------------------------------
# Build shared lib by default ::Adapted from 3D Slicer https://github.com/Slicer/Slicer
#------------------------------------------------------------------------------
option(BUILD_SHARED_LIBS "Build ResectionSegmentsExperiment with shared libraries." ON)
set(BUILD_SHARED_LIBS ON)
mark_as_superbuild(BUILD_SHARED_LIBS:BOOL)
set(${PROJECT_NAME}_BUILD_SHARED ${BUILD_SHARED_LIBS})

#-----------------------------------------------------------------------------
# Build external projects
#-----------------------------------------------------------------------------
set(SUPERBUILD_TOPLEVEL_PROJECT ${PROJECT_NAME})
list(APPEND EXTERNAL_PROJECT_ADDITIONAL_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild)
include(ExternalProject)
include(ExternalProjectDependency)

#-----------------------------------------------------------------------------
# Superbuild script
#-----------------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/SuperBuild")
if(${PROJECT_NAME}_SUPERBUILD)
  include("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake")
  return()
endif()

#-----------------------------------------------------------------------------
# Output directories.
#------------------------------------------------------------------------------
set(${PROJECT_NAME}_BIN_DIR "bin")
set(${PROJECT_NAME}_LIB_DIR "lib")
set(${PROJECT_NAME}_BINARY_DIR "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${${PROJECT_NAME}_BINARY_DIR}/${${PROJECT_NAME}_BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${${PROJECT_NAME}_BINARY_DIR}/${${PROJECT_NAME}_BIN_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${${PROJECT_NAME}_BINARY_DIR}/${${PROJECT_NAME}_LIB_DIR})

# This directory is used to host infrastructure test files
set(CMAKE_TEST_FILES_DIR "${CMAKE_CURRENT_LIST_DIR}/CMake/tests")

# This directory is used to host the cmake infrastructure files
set(CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/CMake")

set(ITK_USE_KWSTYLE OFF)
set(ITK_USE_CLANGFORMAT OFF)

#------------------------------------------------------------------------------
# Add sub-projects
#------------------------------------------------------------------------------
# if(${PROJECT_NAME}_BUILD_ImageIncrement)
#   add_subdirectory(ImageIncrement)
# endif()
