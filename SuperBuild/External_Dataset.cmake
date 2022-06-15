#
# This file is an adaptation of the original adopted from 3D Slicer https://github.com/Slicer/Slicer
#
set(proj Dataset)

# Include dependent projects if any
ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)

ExternalProject_SetIfNotDefined(
  ${PROJECT_NAME}_${proj}_GIT_REPOSITORY
  "${EP_GIT_PROTOCOL}://github.com/ALive-research/DistanceMapsEvaluationDataSet.git"
  QUIET
  )

ExternalProject_SetIfNotDefined(
  ${PROJECT_NAME}_${proj}_GIT_TAG
  "89a1164f5180d9fd175ec3c981eb533d37b6595a"
  QUIET
  )

set(EXTERNAL_PROJECT_OPTIONAL_CMAKE_CACHE_ARGS)

set(EP_SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj})
set(EP_BINARY_DIR ${CMAKE_BINARY_DIR}/${proj}-binary)

ExternalProject_Add(${proj}
  ${${proj}_EP_ARGS}
  GIT_REPOSITORY "${${PROJECT_NAME}_${proj}_GIT_REPOSITORY}"
  GIT_TAG "${${PROJECT_NAME}_${proj}_GIT_TAG}"
  SOURCE_DIR ${EP_SOURCE_DIR}
  BINARY_DIR ${EP_BINARY_DIR}
  CONFIGURE_COMMAND ""
  INSTALL_COMMAND ""
  BUILD_COMMAND ""
  DEPENDS
  ${${proj}_DEPENDENCIES}
  )

# Set the ITK_DIR directory so other projects can find it
set(Dataset_DIR ${EP_SOURCE_DIR})
set(Dataset_BINARY_DIR ${EP_BINARY_DIR})

mark_as_superbuild(VARS Dataset_DIR:PATH LABELS "FIND_PACKAGE")
mark_as_superbuild(VARS Dataset_BINARY_DIR:PATH)
