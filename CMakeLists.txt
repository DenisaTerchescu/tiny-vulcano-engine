cmake_minimum_required(VERSION 3.5.1)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
else()
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

project("TinyVulcano2")

set(ENV{VULKAN_SDK} "D:/Vulkan")

# Move find_package command after project
find_package(Vulkan REQUIRED)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(thirdparty/glfw-3.3.2)            # window opener
add_subdirectory(thirdparty/stb_image)              # loading images
add_subdirectory(thirdparty/glm)                    # math

# Define MY_SOURCES to be a list of all the source files for my game 
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable("TinyVulcano2" ${MY_SOURCES})

target_compile_definitions("TinyVulcano2" PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")
target_include_directories("TinyVulcano2" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_include_directories("TinyVulcano2" PUBLIC "D:/Vulkan/Include/")

target_link_libraries("TinyVulcano2" PRIVATE glm glfw stb_image ${Vulkan_LIBRARIES})