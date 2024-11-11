#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

GLFWwindow *create_window(int width, int height, const char *title);
void destroy_window(GLFWwindow *window);

#endif // !
