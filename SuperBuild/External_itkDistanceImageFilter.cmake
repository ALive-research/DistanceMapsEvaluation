#
# This file is an adaptation of the original adopted from 3D Slicer https://github.com/Slicer/Slicer
#
set(proj itkDistanceImageFilter)

list(APPEND ${proj}_DEPENDENCIES ITK TCLAP)

# Include dependent projects if any
ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)


ExternalProject_SetIfNotDefined(
  ${PROJECT_NAME}_${proj}_GIT_REPOSITORY
  "${EP_GIT_PROTOCOL}://github.com/ALive-research/itkDistanceImageFilter.git"
  QUIET
  )

ExternalProject_SetIfNotDefined(
  ${PROJECT_NAME}_${proj}_GIT_TAG
  "8f0a14df5a37ed3ebcb452a97adaef9010052530"
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
  -DitkDistanceImageFilter_CXX_OPTIMIZATION_FLAGS:STRING= # Force compiler-default instruction set to ensure compatibility with older CPUs
  -DitkDistanceImageFilter_C_OPTIMIZATION_FLAGS:STRING=  # Force compiler-default instruction set to ensure compatibility with older CPUs
  -DitkDistanceImageFilter_INSTALL_ARCHIVE_DIR:PATH=${${PROJECT_NAME}_INSTALL_LIB_DIR}
  -DitkDistanceImageFilter_INSTALL_LIBRARY_DIR:PATH=${${PROJECT_NAME}_INSTALL_LIB_DIR}
  -DITK_DIR:PATH=${ITK_DIR}
  -DTCLAP_DIR:PATH=${TCLAP_DIR}
  -DGIT_EXECUTABLE:FILEPATH=${GIT_EXECUTABLE} # Used in itkDistanceImageFilterModuleRemote
  -DBUILD_SHARED_LIBS:BOOL=ON
  -DBUILD_TESTING:BOOL=${BUILD_TESTING}
  ${EXTERNAL_PROJECT_OPTIONAL_CMAKE_CACHE_ARGS}
  INSTALL_COMMAND ""
  DEPENDS
  ${${proj}_DEPENDENCIES}
  )

# Set the itkDistanceImageFilter_DIR directory so other projects can find it
set(itkDistanceImageFilter_DIR ${EP_BINARY_DIR})

mark_as_superbuild(
  VARS itkDistanceImageFilter_DIR:PATH
  LABELS "FIND_PACKAGE"
  )
