cmake_minimum_required(VERSION 3.16)

project(PTTAT-DM-TTSS VERSION 0.5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC" )

# ----------------------------------------------
# Data structure

add_subdirectory(modules/PTTAT-structure)

# Include structure
if(DEFINED PTTAT_S_exist)
    include_directories(${PTTAT_S_PATH}/include)
    include_directories(${PTTAT_S_PATH}/libraries)

else()
    include(ExternalProject)
    ExternalProject_Add(PTTAT_Structure
            GIT_REPOSITORY https://github.com/FilipCysEk/PTTAT-structure.git
            CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/modules
            )
    include_directories(modules/PTTAT-structure/include)
    include_directories(modules/PTTAT-structure/libraries)

    set(PTTAT_S_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules/PTTAT-structure)

endif()

# ----------------------------------------------
# Libraries

include_directories(libraries)

add_subdirectory(libraries)

include_directories(include)

# ----------------------------------------------
# Start library

add_library(PTTAT-DM-TTSS SHARED
        src/TTSSdataDownloader.cpp
        src/TTSSdataParser.cpp
        src/TTSSdirector.cpp
        ${PTTAT_S_PATH}/src/Logger.cpp)

target_precompile_headers(PTTAT-DM-TTSS PUBLIC ${PTTAT_S_PATH}/include/rapidjson_pch.h)

target_link_libraries(PTTAT-DM-TTSS PUBLIC fileLib)
target_link_libraries(PTTAT-DM-TTSS PUBLIC configLib)
target_link_libraries(PTTAT-DM-TTSS PUBLIC downloaderLib)
target_link_libraries(PTTAT-DM-TTSS PUBLIC PTTAT_structure)
target_link_libraries(PTTAT-DM-TTSS PUBLIC spdlog::spdlog)

set_target_properties(PTTAT-DM-TTSS PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)