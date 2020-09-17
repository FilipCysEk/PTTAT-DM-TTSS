# curl
set(CURL_LIBRARY "-lcurl")
find_package(CURL REQUIRED)

#add_library(downloaderLib STATIC IMPORTED)
#set_property(TARGET downloaderLib PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/libDownloaderLib.a)
#target_link_libraries(downloaderLib INTERFACE stdc++fs)
#target_link_libraries(downloaderLib INTERFACE ${CURL_LIBRARIES})

#add_library(downloaderLib ${CMAKE_CURRENT_SOURCE_DIR}/cygDownloaderLibShared.dll)
#set_target_properties(downloaderLib PROPERTIES LINKER_LANGUAGE CXX)

if (NOT TARGET downloaderLib)
    add_library(downloaderLib INTERFACE)
    target_link_libraries(downloaderLib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/libDownloaderLib.a)
    target_link_libraries(downloaderLib INTERFACE stdc++fs)
    target_link_libraries(downloaderLib INTERFACE ${CURL_LIBRARIES})
endif ()