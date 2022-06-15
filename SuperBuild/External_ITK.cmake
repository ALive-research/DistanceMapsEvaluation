#
# This file is an adaptation of the original adopted from 3D Slicer https://github.com/Slicer/Slicer
#
set(proj ITK)

set(${proj}_DEPENDENCIES VTK)

# Include dependent projects if any
ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)

if(${PROJECT_NAME}_USE_SYSTEM_${proj})
  unset(ITK_DIR CACHE)
  find_package(ITK 5.2 REQUIRED NO_MODULE)
endif()

# Sanity checks
if(DEFINED ITK_DIR AND NOT EXISTS ${ITK_DIR})
  message(FATAL_ERROR "ITK_DIR variable is defined but corresponds to nonexistent directory")
endif()

if(NOT DEFINED ITK_DIR AND NOT ${PROJECT_NAME}_USE_SYSTEM_${proj})

  ExternalProject_SetIfNotDefined(
    ${PROJECT_NAME}_${proj}_GIT_REPOSITORY
    "${EP_GIT_PROTOCOL}://github.com/InsightSoftwareConsortium/ITK"
    QUIET
    )

  ExternalProject_SetIfNotDefined(
    ${PROJECT_NAME}_${proj}_GIT_TAG
    "v5.2.1"
    QUIET
    )

  set(EXTERNAL_PROJECT_OPTIONAL_CMAKE_CACHE_ARGS)

  # Enable GPU SUPPORT if requested
  list(APPEND EXTERNAL_PROJECT_OPTIONAL_CMAKE_CACHE_ARGS
    "-DITK_USE_GPU:BOOL=${${PROJECT_NAME}_BUILD_GPU}")

  set(EP_SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj})
  set(EP_BINARY_DIR ${CMAKE_BINARY_DIR}/${proj}-build)

  ExternalProject_Add(${proj}
    ${${proj}_EP_ARGS}
    GIT_REPOSITORY "${${PROJECT_NAME}_${proj}_GIT_REPOSITORY}"
    GIT_TAG "${${PROJECT_NAME}_${proj}_GIT_TAG}"
    SOURCE_DIR ${EP_SOURCE_DIR}
    BINARY_DIR ${EP_BINARY_DIR}
    CMAKE_CACHE_ARGS
      -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
      -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
      -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
      -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
      -DCMAKE_CXX_STANDARD:STRING=${CMAKE_CXX_STANDARD}
      -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=${CMAKE_CXX_STANDARD_REQUIRED}
      -DCMAKE_CXX_EXTENSIONS:BOOL=${CMAKE_CXX_EXTENSIONS}
      -DITK_CXX_OPTIMIZATION_FLAGS:STRING= # Force compiler-default instruction set to ensure compatibility with older CPUs
      -DITK_C_OPTIMIZATION_FLAGS:STRING=  # Force compiler-default instruction set to ensure compatibility with older CPUs
      -DITK_INSTALL_ARCHIVE_DIR:PATH=${${PROJECT_NAME}_INSTALL_LIB_DIR}
      -DITK_INSTALL_LIBRARY_DIR:PATH=${${PROJECT_NAME}_INSTALL_LIB_DIR}
      -DBUILD_EXAMPLES:BOOL=OFF
      -DBUILD_TESTING:BOOL=${BUILD_TESTING}
      -DITK_BUILD_DEFAULT_MODULES:BOOL=OFF
      -DITKGroup_Filtering:BOOL=ON
      -DITKGroup_IO:BOOL=ON
      -DModule_ITKVtkGlue:BOOL=ON
      -DGIT_EXECUTABLE:FILEPATH=${GIT_EXECUTABLE} # Used in ITKModuleRemote
      -DBUILD_SHARED_LIBS:BOOL=ON
      -DITK_INSTALL_NO_DEVELOPMENT:BOOL=ON
      -DITK_WRAPPING:BOOL=OFF #${BUILD_SHARED_LIBS} ## HACK:  QUICK CHANGE
      -DVTK_DIR:PATH=${VTK_DIR}
      ${EXTERNAL_PROJECT_OPTIONAL_CMAKE_CACHE_ARGS}
    INSTALL_COMMAND ""
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )

  # Set the ITK_DIR directory so other projects can find it
  set(ITK_DIR ${EP_BINARY_DIR})

else()
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

mark_as_superbuild(
  VARS ITK_DIR:PATH
  LABELS "FIND_PACKAGE"
  )
