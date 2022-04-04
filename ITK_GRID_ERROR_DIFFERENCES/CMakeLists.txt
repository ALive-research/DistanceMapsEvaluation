# This is the root ITK CMakeLists file.
cmake_minimum_required(VERSION 3.9.5 FATAL_ERROR)
foreach(p
    ## Only policies introduced after the cmake_minimum_required
    ## version need to explicitly be set to NEW.
    CMP0070 #3.10.0 Define ``file(GENERATE)`` behavior for relative paths.
    CMP0071 #3.10.0 Let ``AUTOMOC`` and ``AUTOUIC`` process ``GENERATED`` files.
    )
  if(POLICY ${p})
    cmake_policy(SET ${p} NEW)
  endif()
endforeach()


# This project is designed to be built outside the Insight source tree.
project(GridOctreeMT)

# Find ITK.
set( ITK_DIR "~/src_build/InsightToolkit-5.1.2" )
find_package(ITK REQUIRED)
IF(ITK_FOUND)
include(${ITK_USE_FILE})
ENDIF(ITK_FOUND)


add_executable(itkGrid itkGrid.cpp includes/ComputeDistancesGrid.hxx)

target_link_libraries(itkGrid ${ITK_LIBRARIES})
