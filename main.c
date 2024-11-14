#include "window.h"
#include "vulkan_fn.h"

void clean_up(vk_t *vk, GLFWwindow *window)
{
  if (ENABLE_VALIDATION_LAYERS)
  {
    destroy_debug_utils_messenger_ext(vk->instance, vk->debug_messenger, NULL);
  }

  vkDestroyInstance(vk->instance, NULL);
  destroy_window(window);
}

int main()
{
  glfwInit();
  GLFWwindow *window = create_window(800, 600, "Vuck");
  vk_t vk = create_instance();
  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
  }

  clean_up(&vk, window);
  return 0;
}
