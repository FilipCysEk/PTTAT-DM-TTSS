cmake_minimum_required(VERSION 3.16)


# Include structure
if(DEFINED PTTAT_S_exist)
    include_directories(${PTTAT_S_PATH}/include)
    include_directories(${PTTAT_S_PATH}/libraries)
    include_directories(${PTTAT_S_PATH}/DataManager/includePublic)

else()
    ExternalProject_Add(PTTAT_Structure
            GIT_REPOSITORY https://github.com/FilipCysEk/PTTAT-structure.git
            CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/modules
            )
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/modules/PTTAT-structure/include)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/modules/PTTAT-structure/libraries)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/modules/PTTAT-structure/DataManager/includePublic)

    set(PTTAT_S_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules/PTTAT-structure)

endif()

# adding slibraries
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libraries)

add_subdirectory(libraries)

include_directories(include)

# ----------------------------------------------
# Start app

project(TTSS)

set(CMAKE_CXX_STANDARD 17)
set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC" )

add_library(ttss SHARED src/TTSSdataDownloader.cpp src/TTSSdataParser.cpp src/TTSSdirector.cpp ${PTTAT_S_PATH}/src/Logger.cpp)

target_precompile_headers(ttss PUBLIC ${PTTAT_S_PATH}/include/rapidjson_pch.h)
#target_precompile_headers(ttss PUBLIC ${CMAKE_SOURCE_DIR}/include/spdlog_pch.h)
target_precompile_headers(ttss PUBLIC ${PTTAT_S_PATH}/libraries/configLib/ConfigFile.h)
target_precompile_headers(ttss PUBLIC ${PTTAT_S_PATH}/libraries/fileLib/FileLib.h)
target_precompile_headers(ttss PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libraries/downloaderLib/Downloader.h)

target_link_libraries(ttss PRIVATE DataManager)
target_link_libraries(ttss PUBLIC fileLib)
target_link_libraries(ttss PUBLIC configLib)
target_link_libraries(ttss PUBLIC downloaderLib)
target_link_libraries(ttss PRIVATE spdlog::spdlog)

set_target_properties(ttss PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)