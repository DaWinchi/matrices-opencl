cmake_minimum_required(VERSION 3.0)
set(PROJECT_NAME matrices-opencl)
project(${PROJECT_NAME} C CXX)
find_package(OpenCL REQUIRED)
find_package(OpenMP REQUIRED)
include_directories( ${OPENCL_INCLUDE_DIR} )
link_directories(${OpenCL_LIBRARIES})
set(HEADERS 
            inc/Computing.h)
set(SOURCES
            src/main.cpp
            src/Computing.cpp)
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries( ${PROJECT_NAME} OpenCL::OpenCL OpenMP::OpenMP_CXX)