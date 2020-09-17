include_directories(include ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(ttss SHARED src/TTSSdataDownloader.cpp src/TTSSdataParser.cpp src/TTSSdirector.cpp ../../TramHoldupBasics/src/Logger.cpp)

target_precompile_headers(ttss PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../TramHoldupBasics/include/rapidjson_pch.h)
#target_precompile_headers(ttss PUBLIC ${CMAKE_SOURCE_DIR}/include/spdlog_pch.h)
target_precompile_headers(ttss PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../TramHoldupBasics/libraries/configLib/ConfigFile.h)
target_precompile_headers(ttss PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../TramHoldupBasics/libraries/fileLib/FileLib.h)
target_precompile_headers(ttss PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../libraries/downloaderLib/Downloader.h)

target_link_libraries(ttss PRIVATE DataManager)
target_link_libraries(ttss PUBLIC fileLib)
target_link_libraries(ttss PUBLIC configLib)
target_link_libraries(ttss PUBLIC downloaderLib)
target_link_libraries(ttss PRIVATE spdlog::spdlog)

set_target_properties(ttss PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)