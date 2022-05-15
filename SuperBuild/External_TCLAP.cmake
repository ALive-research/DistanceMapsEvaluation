include(ExternalProject)

set(proj TCLAP)

if(${PROJECT_NAME}_USE_SYSTEM_${proj})
  unset(TCLAP_DIR CACHE)
  find_package(TCLAP 1.4.2 REQUIRED)
endif()

# Sanity checks
if(DEFINED TCLAP_DIR AND NOT EXISTS ${TCLAP_DIR})
  message(FATAL_ERROR "TCLAP_DIR variable is defined but corresponds to nonexistent directory")
endif()

if(NOT DEFINED TCLAP_DIR AND NOT ${PROJECT_NAME}_USE_SYSTEM_${proj})

  ExternalProject_SetIfNotDefined(
    ${PROJECT_NAME}_${proj}_GIT_REPOSITORY
    "${EP_GIT_PROTOCOL}://git.code.sf.net/p/tclap/code"
    QUIET
    )

  ExternalProject_SetIfNotDefined(
    ${PROJECT_NAME}_${proj}_GIT_TAG
    "v1.2.4"
    QUIET
    )

  set(EXTERNAL_PROJECT_OPTIONAL_CMAKE_CACHE_ARGS)

  set(EP_SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj})

  ExternalProject_Add(${proj}
    ${${proj}_EP_ARGS}
    GIT_REPOSITORY "${${PROJECT_NAME}_${proj}_GIT_REPOSITORY}"
    GIT_TAG "${${PROJECT_NAME}_${proj}_GIT_TAG}"
    SOURCE_DIR ${EP_SOURCE_DIR}
    BINARY_DIR ""
    CMAKE_CACHE_ARGS
    ${EXTERNAL_PROJECT_OPTIONAL_CMAKE_CACHE_ARGS}
    -DBUILD_TESTING=OFF
    INSTALL_COMMAND ""
    DEPENDS
    ${${proj}_DEPENDENCIES}
    )

  set(TCLAP_DIR ${EP_SOURCE_DIR})
else()
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

mark_as_superbuild(
  VARS TCLAP_DIR:PATH
  LABELS "FIND_PACKAGE"
  )
