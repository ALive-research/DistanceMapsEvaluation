#
# This file is an adaptation of the original adopted from 3D Slicer https://github.com/Slicer/Slicer
#
set(proj itkSphereImageGenerator)

list(APPEND ${proj}_DEPENDENCIES ITK TCLAP)

# Include dependent projects if any
ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)

ExternalProject_SetIfNotDefined(
  ${PROJECT_NAME}_${proj}_GIT_REPOSITORY
  "${EP_GIT_PROTOCOL}://github.com/ALive-research/itkSphereImageGenerator.git"
  QUIET
  )

ExternalProject_SetIfNotDefined(
  ${PROJECT_NAME}_${proj}_GIT_TAG
  "f62ffed9bbfdec109dafb2193948a10eaa0d365e"
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
  -DitkSphereImageGenerator_CXX_OPTIMIZATION_FLAGS:STRING= # Force compiler-default instruction set to ensure compatibility with older CPUs
  -DitkSphereImageGenerator_C_OPTIMIZATION_FLAGS:STRING=  # Force compiler-default instruction set to ensure compatibility with older CPUs
  -DitkSphereImageGenerator_INSTALL_ARCHIVE_DIR:PATH=${${PROJECT_NAME}_INSTALL_LIB_DIR}
  -DitkSphereImageGenerator_INSTALL_LIBRARY_DIR:PATH=${${PROJECT_NAME}_INSTALL_LIB_DIR}
  -DITK_DIR:PATH=${ITK_DIR}
  -DTCLAP_DIR:PATH=${TCLAP_DIR}
  -DGIT_EXECUTABLE:FILEPATH=${GIT_EXECUTABLE} # Used in itkSphereImageGeneratorModuleRemote
  -DBUILD_SHARED_LIBS:BOOL=ON
  ${EXTERNAL_PROJECT_OPTIONAL_CMAKE_CACHE_ARGS}
  INSTALL_COMMAND ""
  DEPENDS
  ${${proj}_DEPENDENCIES}
  )

# Set the itkSphereImageGenerator_DIR directory so other projects can find it
set(itkSphereImageGenerator_DIR ${EP_BINARY_DIR})

mark_as_superbuild(
  VARS itkSphereImageGenerator_DIR:PATH
  LABELS "FIND_PACKAGE"
  )
