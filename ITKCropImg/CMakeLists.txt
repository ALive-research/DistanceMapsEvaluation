cmake_minimum_required(VERSION 3.10.2)

project(itkVtkDistances)

include_directories(.)

# Find ITK.
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

#find_package(VTK 9.0 REQUIRED)

#add_executable(itkGridDistance itkGridDistance.cxx )
#target_link_libraries(itkGridDistance ${ITK_LIBRARIES})

#add_executable(itkObjectDistance itkObjectDistance.cxx )
#target_link_libraries(itkObjectDistance ${ITK_LIBRARIES})

add_executable(itkCrop itkCrop.cxx)
target_link_libraries(itkCrop ${ITK_LIBRARIES} ${VTK_LIBRARIES})

add_executable(itkCheck itkCheck.cxx)
target_link_libraries(itkCheck ${ITK_LIBRARIES} ${VTK_LIBRARIES})
