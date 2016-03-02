cmake_minimum_required(VERSION 2.8.12.1)

PROJECT(project1F)
SET(VTK_DIR /home/eewing/VTK-Builds/Standard/lib/cmake/vtk-6.1)
SET(CMAKE_BUILD_TYPE "Debug")

find_package(VTK REQUIRED)
find_package(Threads REQUIRED)
include(${VTK_USE_FILE})
include_directories(.)

add_executable(project1F project1F.cxx Triangle.cxx Screen.cxx reader1F.cxx Vector.cxx Camera.cxx Matrix.cxx)

if(VTK_LIBRARIES)
  target_link_libraries(project1F ${VTK_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
else()
  target_link_libraries(project1F vtkHybrid ${CMAKE_THREAD_LIBS_INIT})
endif()

