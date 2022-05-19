#
# This file is based on the original adopted from 3D Slicer https://github.com/Slicer/Slicer

#-----------------------------------------------------------------------------
# Git protocol option
#-----------------------------------------------------------------------------
if(EP_GIT_PROTOCOL STREQUAL "https")
  # Verify that the global git config has been updated with the expected "insteadOf" option.
  # XXX CMake 3.8: Replace this with use of GIT_CONFIG option provided by ExternalProject
  function(_check_for_required_git_config_insteadof base insteadof)
    execute_process(
      COMMAND ${GIT_EXECUTABLE} config --global --get "url.${base}.insteadof"
      OUTPUT_VARIABLE output
      OUTPUT_STRIP_TRAILING_WHITESPACE
      RESULT_VARIABLE error_code
      )
    if(error_code OR NOT "${output}" STREQUAL "${insteadof}")
      message(FATAL_ERROR
"Since the ExternalProject modules doesn't provide a mechanism to customize the clone step by "
"adding 'git config' statement between the 'git checkout' and the 'submodule init', it is required "
"to manually update your global git config to successfully build ${CMAKE_PROJECT_NAME} with "
"option MIBBenchmark_USE_GIT_PROTOCOL set to FALSE. "
"See http://na-mic.org/Mantis/view.php?id=2731"
"\nYou could do so by running the command:\n"
"  ${GIT_EXECUTABLE} config --global url.${base}.insteadOf ${insteadof}\n")
    endif()
  endfunction()

endif()

#-----------------------------------------------------------------------------
# Enable and setup External project global properties
#-----------------------------------------------------------------------------

set(ep_common_c_flags "${CMAKE_C_FLAGS_INIT} ${ADDITIONAL_C_FLAGS}")
set(ep_common_cxx_flags "${CMAKE_CXX_FLAGS_INIT} ${ADDITIONAL_CXX_FLAGS}")

#-----------------------------------------------------------------------------
# Define list of additional options used to configure MIBBenchmark
#------------------------------------------------------------------------------

if(DEFINED CTEST_CONFIGURATION_TYPE)
  mark_as_superbuild(CTEST_CONFIGURATION_TYPE)
endif()

if(DEFINED CMAKE_CONFIGURATION_TYPES)
  mark_as_superbuild(CMAKE_CONFIGURATION_TYPES)
endif()

#------------------------------------------------------------------------------
# Dependency list
#------------------------------------------------------------------------------

set(itkSphereImageGenerator_EXTERNAL_NAME itkSphereImageGenerator)
set(itkRadialDistance_EXTERNAL_NAME itkRadialDistance)
set(itkMaurerDistance_EXTERNAL_NAME itkMaurerDistance)
set(itkImageCompare_EXTERNAL_NAME itkImageCompare)
set(itkDistanceImageFilter_EXTERNAL_NAME itkDistanceImageFilter)
set(ITK_EXTERNAL_NAME ITK)

set(${PROJECT_NAME}_DEPENDENCIES
  ${ITK_EXTERNAL_NAME}
  ${itkMaurerDistance_EXTERNAL_NAME}
  ${itkImageCompare_EXTERNAL_NAME}
  ${itkRadialDistance_EXTERNAL_NAME}
  ${itkSphereImageGenerator_EXTERNAL_NAME}
  ${itkDistanceImageFilter_EXTERNAL_NAME}
  )
#------------------------------------------------------------------------------
# REsectionSegmentsExperiment_ADDITIONAL_PROJECTS
#------------------------------------------------------------------------------

list(APPEND ${PROJECT_NAME}_ADDITIONAL_PROJECTS ${${PROJECT_NAME}_ADDITIONAL_DEPENDENCIES})
if(${PROJECT_NAME}_ADDITIONAL_PROJECTS)
  list(REMOVE_DUPLICATES ${PROJECT_NAME}_ADDITIONAL_PROJECTS)
  foreach(additional_project ${${PROJECT_NAME}_ADDITIONAL_PROJECTS})
    # needed to do find_package within ${PROJECT_NAME}
    mark_as_superbuild(${additional_project}_DIR:PATH)
  endforeach()
  mark_as_superbuild(${PROJECT_NAME}_ADDITIONAL_PROJECTS:STRING)
endif()

#------------------------------------------------------------------------------
# Slicer_ADDITIONAL_DEPENDENCIES, EXTERNAL_PROJECT_ADDITIONAL_DIR, EXTERNAL_PROJECT_ADDITIONAL_DIRS
#------------------------------------------------------------------------------

if(DEFINED ${PROJECT_NAME}_ADDITIONAL_DEPENDENCIES)
  list(APPEND ${PROJECT_NAME}_DEPENDENCIES ${${PROJECT_NAME}_ADDITIONAL_DEPENDENCIES})
endif()

mark_as_superbuild(${PROJECT_NAME}_DEPENDENCIES:STRING)

#------------------------------------------------------------------------------
# Process external projects, aggregate variable marked as superbuild and set <proj>_EP_ARGS variable.
#------------------------------------------------------------------------------

ExternalProject_Include_Dependencies(${PROJECT_NAME} DEPENDS_VAR ${PROJECT_NAME}_DEPENDENCIES)

#------------------------------------------------------------------------------
# Configure and build
#------------------------------------------------------------------------------
set(proj ${PROJECT_NAME})

ExternalProject_Add(${proj}
  ${${proj}_EP_ARGS}
  DEPENDS ${${PROJECT_NAME}_DEPENDENCIES} ${${PROJECT_NAME}_REMOTE_DEPENDENCIES}
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  BINARY_DIR ${CMAKE_BINARY_DIR}/${${PROJECT_NAME}_BINARY_INNER_SUBDIR}
  DOWNLOAD_COMMAND ""
  UPDATE_COMMAND ""
  CMAKE_CACHE_ARGS
    -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
    -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
    -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
    -DCMAKE_CXX_STANDARD:STRING=${CMAKE_CXX_STANDARD}
    -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=${CMAKE_CXX_STANDARD_REQUIRED}
    -DCMAKE_CXX_EXTENSIONS:BOOL=${CMAKE_CXX_EXTENSIONS}
    -DADDITIONAL_C_FLAGS:STRING=${ADDITIONAL_C_FLAGS}
    -DADDITIONAL_CXX_FLAGS:STRING=${ADDITIONAL_CXX_FLAGS}
    -DBUILD_TESTING:BOOL=${BUILD_TESTING}
    -D${PROJECT_NAME}_REQUIRED_C_FLAGS:STRING=${${PROJECT_NAME}_REQUIRED_C_FLAGS}
    -D${PROJECT_NAME}_REQUIRED_CXX_FLAGS:STRING=${${PROJECT_NAME}_REQUIRED_CXX_FLAGS}
    -D${PROJECT_NAME}_SUPERBUILD:BOOL=OFF
    -D${PROJECT_NAME}_SUPERBUILD_DIR:PATH=${CMAKE_BINARY_DIR}
    -D${${PROJECT_NAME}_MAIN_PROJECT}_APPLICATION_NAME:STRING=${${${PROJECT_NAME}_MAIN_PROJECT}_APPLICATION_NAME}
    -D${PROJECT_NAME}_EXTENSION_SOURCE_DIRS:STRING=${${PROJECT_NAME}_EXTENSION_SOURCE_DIRS}
    -D${PROJECT_NAME}_EXTENSION_INSTALL_DIRS:STRING=${${PROJECT_NAME}_EXTENSION_INSTALL_DIRS}
    -DExternalData_OBJECT_STORES:PATH=${ExternalData_OBJECT_STORES}
    ${EXTERNAL_PROJECT_OPTIONAL_ARGS}
  INSTALL_COMMAND ""
  )

ExternalProject_AlwaysConfigure(${proj})
