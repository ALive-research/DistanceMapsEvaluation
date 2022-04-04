cmake_minimum_required(VERSION 3.10.2)

project(itkVtkDistances)

include_directories(.)

# Find ITK.
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_executable(itkBruteForce itkBruteForce.cxx)
target_link_libraries(itkBruteForce ${ITK_LIBRARIES} ${VTK_LIBRARIES})

