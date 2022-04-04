cmake_minimum_required(VERSION 3.10.2)

project(itkVtkDistances)

include_directories(.)

# Find ITK.
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(VTK 9.0 REQUIRED)


set(CMAKE_CXX_FLAGS "-fopenmp")

add_executable(vtkHausdorff vtkHausdorffDist.cxx
  ./HausdorffDistanceOctrees/vtkHausdorffDistanceOctreesFilter.cxx
  ./HausdorffDistancePointLocator/vtkHausdorffDistancePointLocatorFilter.cxx)

target_link_libraries(vtkHausdorff ${ITK_LIBRARIES} ${VTK_LIBRARIES})
