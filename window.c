#include "window.h"

GLFWwindow* create_window(int width, int height, const char* title){
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
  return window;
}

void destroy_window(GLFWwindow* window){
  glfwDestroyWindow(window);
  glfwTerminate();
}
