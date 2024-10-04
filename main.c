#include "window.h"
#include "vulkan_fn.h"

int main(){
  glfwInit();
  GLFWwindow* window = create_window(800, 600, "Vuck");
  create_instance();
  while(!glfwWindowShouldClose(window)){
    glfwPollEvents();
  }
  return 0;
}
