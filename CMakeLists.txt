cmake_minimum_required(VERSION 3.5.1)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
else()
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

project("TinyVulcanoEngine")

set(ENV{VULKAN_SDK} "C:/VulkanSDK/1.3.290.0")

find_package(Vulkan REQUIRED)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(thirdparty/glfw-3.3.2)            
add_subdirectory(thirdparty/stb_image)              
add_subdirectory(thirdparty/glm)                                     
add_subdirectory(thirdparty/imgui-docking)                                     

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable("TinyVulcanoEngine" ${MY_SOURCES})

target_compile_features("TinyVulcanoEngine" PRIVATE cxx_std_17)
target_compile_definitions("TinyVulcanoEngine" PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")
target_include_directories("TinyVulcanoEngine" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_include_directories("TinyVulcanoEngine" PUBLIC "C:/VulkanSDK/1.3.290.0/Include/")

target_link_libraries("TinyVulcanoEngine" PRIVATE glm glfw stb_image imgui ${Vulkan_LIBRARIES})