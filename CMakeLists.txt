cmake_minimum_required(VERSION 3.10)
project(vuck)

set(GLFW_INCLUDE_DIR "D:/glfw-3.4.bin.WIN64/include")
set(GLFW_LIBRARY "D:/glfw-3.4.bin.WIN64/lib-mingw-w64")
set(VULKAN_SDK "D:/VulkanSDK/")

include_directories("${GLFW_INCLUDE_DIR}")
include_directories("${VULKAN_SDK}/Include")
link_directories("${GLFW_LIBRARY}")
link_directories("${VULKAN_SDK}/Lib")


add_executable(vuck main.c window.c vulkan_fn.c)
target_link_libraries(vuck glfw3 vulkan-1)

set_target_properties(vuck PROPERTIES 
COMPILE_DEFINITIONS_DEBUG "DEBUG" 
COMPILE_OPTIONS_DEBUG "-g")