#
# Set minimum required version
#
cmake_minimum_required(VERSION 3.9)

project(PiELo)

# Set the compilation type
if(CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel ..." FORCE)
endif(CMAKE_BUILD_TYPE STREQUAL "")
message(STATUS "Compiling??")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-D__DEBUG_INSTRUCTIONS__ -D__DEBUG_PARSER__ -g)
  message(STATUS "Compiling with debug flag")
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

# Deactivate RPATH for MacOSX
set(CMAKE_MACOSX_RPATH 0)

# Set the path where CMake will find additional scripts
set(CMAKE_MODULE_PATH $ENV{CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_PREFIX_PATH $ENV{CMAKE_PREFIX_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find the ARGoS package, make sure to save the ARGoS prefix
find_package(ARGoS REQUIRED)
include_directories(${CMAKE_SOURCE_DIR} ${ARGOS_INCLUDE_DIRS})
link_directories(${ARGOS_LIBRARY_DIR})
link_libraries(${ARGOS_LDFLAGS})

# Find the ARGoS package, make sure to save the ARGoS prefix
find_package(ARGoS COMPONENTS footbot eyebot spiri)

#
# Include path points to the base source dir
#
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})


add_subdirectory(instructions)
add_subdirectory(argos)

add_library(PiELo SHARED
    exceptions.h
    gc.cpp gc.h
    instructionHandler.cpp instructionHandler.h
    networking.cpp networking.h
    parser.cpp parser.h
    robotFunctions.cpp robotFunctions.h
    vm.cpp vm.h)

add_executable(vm main.cpp)
target_link_libraries(vm PiELo instructions)

add_executable(router router.cpp)
target_link_libraries(router PiELo)
