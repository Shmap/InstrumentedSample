cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
cmake_policy(VERSION 3.6)

# Sources

file(GLOB_RECURSE FILE_SOURCES RELATIVE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

message("===================================")
message(${CMAKE_CURRENT_SOURCE_DIR})
message(${CMAKE_BINARY_DIR})
message("===================================")

# Finalize App
add_executable(
    ${PROJECT_NAME}
	WIN32
    "${FILE_SOURCES}"
)

# Finish Dependencies
target_link_libraries(${PROJECT_NAME} d3d12.lib dxgi.lib d3dcompiler.lib ittnotify)

# Copy shaders to binaries
file(COPY ./shaders.hlsl
	DESTINATION ${CMAKE_BINARY_DIR}/bin
	)