#
# This file is an adaptation of the original adopted from 3D Slicer https://github.com/Slicer/Slicer
#
set(proj VTK)

# Include dependent projects if any
ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)

if(${PROJECT_NAME}_USE_SYSTEM_${proj})
  unset(VTK_DIR CACHE)
  find_package(VTK 9.1.0 REQUIRED NO_MODULE)
endif()

# Sanity checks
if(DEFINED VTK_DIR AND NOT EXISTS ${VTK_DIR})
  message(FATAL_ERROR "VTK_DIR variable is defined but corresponds to nonexistent directory")
endif()

if(NOT DEFINED VTK_DIR AND NOT ${PROJECT_NAME}_USE_SYSTEM_${proj})

  ExternalProject_SetIfNotDefined(
    ${PROJECT_NAME}_${proj}_GIT_REPOSITORY
    "${EP_GIT_PROTOCOL}://github.com/kitware/vtk.git"
    QUIET
    )

  ExternalProject_SetIfNotDefined(
    ${PROJECT_NAME}_${proj}_GIT_TAG
    "v9.1.0"
    QUIET
    )

  set(EXTERNAL_PROJECT_OPTIONAL_CMAKE_CACHE_ARGS)

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
      -DVTK_CXX_OPTIMIZATION_FLAGS:STRING= # Force compiler-default instruction set to ensure compatibility with older CPUs
      -DVTK_C_OPTIMIZATION_FLAGS:STRING=  # Force compiler-default instruction set to ensure compatibility with older CPUs
      -DVTK_INSTALL_ARCHIVE_DIR:PATH=${${PROJECT_NAME}_INSTALL_LIB_DIR}
      -DVTK_INSTALL_LIBRARY_DIR:PATH=${${PROJECT_NAME}_INSTALL_LIB_DIR}
      -DBUILD_TESTING:BOOL=OFF
      -DBUILD_EXAMPLES:BOOL=OFF
      -DGIT_EXECUTABLE:FILEPATH=${GIT_EXECUTABLE} # Used in VTKModuleRemote
      -DBUILD_SHARED_LIBS:BOOL=ON
      ${EXTERNAL_PROJECT_OPTIONAL_CMAKE_CACHE_ARGS}
    INSTALL_COMMAND ""
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )

  # Set the VTK_DIR directory so other projects can find it
  set(VTK_DIR ${EP_BINARY_DIR})

else()
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

mark_as_superbuild(
  VARS VTK_DIR:PATH
  LABELS "FIND_PACKAGE"
  )
