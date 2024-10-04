cmake_minimum_required(VERSION 3.10..3.27)
project(Vuck)

add_executable(main main.c window.c vulkan_fn.c)
target_link_libraries(main /usr/lib/libvulkan.so /usr/lib/libglfw.so
                      /usr/lib/libm.so)
