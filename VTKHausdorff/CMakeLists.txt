cmake_minimum_required(VERSION 3.10.2)

project(itkVtkDistances)

include_directories(.)

# Find ITK.
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(VTK 9.0 REQUIRED)

add_executable(vtkHausdorffDist vtkHausdorffDist.cxx)
target_link_libraries(vtkHausdorffDist ${ITK_LIBRARIES} ${VTK_LIBRARIES})
