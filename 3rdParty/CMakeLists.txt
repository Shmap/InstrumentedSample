cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

#list(APPEND CMAKE_MODULE_PATH
#    "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules"
#	)

add_subdirectory(IttApi)

#file(GLOB_RECURSE FILE_SOURCES RELATIVE
#    ${CMAKE_CURRENT_SOURCE_DIR}
#    ${CMAKE_CURRENT_SOURCE_DIR}/IttApi/src/*.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/IttApi/src/*.h
#	)

#set_target_properties(ittnotify
#    PROPERTIES
#    FOLDER 3rdParty
#)