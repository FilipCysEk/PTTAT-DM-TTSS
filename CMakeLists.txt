cmake_minimum_required(VERSION 3.16)
project(TTSS)

set(CMAKE_CXX_STANDARD 17)
set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC" )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/TramHoldupBasics/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/TramHoldupBasics/libraries)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/TramHoldupBasics/DataManager/includePublic)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libraries)

add_subdirectory(TramHoldupBasics)
add_subdirectory(libraries)
add_subdirectory(TTSS)