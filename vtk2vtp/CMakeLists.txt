
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(vtk2vtp)

find_package(VTK COMPONENTS 
  vtkIOLegacy
  vtkIOXML
  QUIET
)

if (NOT VTK_FOUND)
  message("Skipping GenericDataObjectReader: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif()
message (STATUS "VTK_VERSION: ${VTK_VERSION}")
if (VTK_VERSION VERSION_LESS "8.90.0")
  # old system
  include(${VTK_USE_FILE})
  add_executable(vtk2vtp MACOSX_BUNDLE vtk2vtp.cxx )
  target_link_libraries(vtk2vtp PRIVATE ${VTK_LIBRARIES})
else ()
  # include all components
  add_executable(vtk2vtp MACOSX_BUNDLE vtk2vtp.cxx )
  target_link_libraries(vtk2vtp PRIVATE ${VTK_LIBRARIES})
  # vtk_module_autoinit is needed
  vtk_module_autoinit(
    TARGETS vtk2vtp
    MODULES ${VTK_LIBRARIES}
    )
endif ()
