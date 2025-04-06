cmake_minimum_required(VERSION 3.15) 

#if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
#else()
#    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
#endif()

project("TinyVulcanoEngine")

#set(CMAKE_MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME} CACHE STRING "" FORCE)
#make shure the runtime library is statically linked
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(ENV{VULKAN_SDK} "C:/VulkanSDK/1.3.290.0")

find_package(Vulkan REQUIRED)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# Configure Assimp
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_NO_EXCEPTIONS ON CACHE BOOL "" FORCE) 
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_EXPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_MTL_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_MTL_EXPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_EXPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTFB_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTFB_EXPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
option(ASSIMP_BUILD_STATIC_LIB "If the library shall be built as static library." ON)
set(ASSIMP_BUILD_ZLIB "1" CACHE STRING "If zlib is used, static linking. Specify Zlib < 1.3")
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)




add_subdirectory(thirdparty/glfw-3.3.2)            
add_subdirectory(thirdparty/stb_image)              
add_subdirectory(thirdparty/glm)                                     
add_subdirectory(thirdparty/imgui-docking)                                     
add_subdirectory(thirdparty/tinyobjloader)       
add_subdirectory(thirdparty/assimp-5.2.4)


file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable("TinyVulcanoEngine" ${MY_SOURCES})

target_compile_features("TinyVulcanoEngine" PRIVATE cxx_std_17)
target_compile_definitions("TinyVulcanoEngine" PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")
target_include_directories("TinyVulcanoEngine" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_include_directories("TinyVulcanoEngine" PUBLIC "C:/VulkanSDK/1.3.290.0/Include/")

target_link_libraries("TinyVulcanoEngine" PRIVATE glm glfw stb_image imgui tinyobjloader assimp ${Vulkan_LIBRARIES})