#include "window.h"
#include "vulkan_fn.h"

void clean_up(vk_t *vk, GLFWwindow *window)
{
  vkDestroyDevice(vk->device, NULL);
  if (ENABLE_VALIDATION_LAYERS)
  {
    destroy_debug_utils_messenger_ext(vk->instance, vk->debug_messenger, NULL);
  }
  vkDestroySurfaceKHR(vk->instance, vk->surface, NULL);
  vkDestroyInstance(vk->instance, NULL);
  destroy_window(window);
}

int main()
{
  glfwInit();
  GLFWwindow *window = create_window(800, 600, "Vuck");

  // VULKAN SECTION START
  vk_t vk = create_instance();
  setup_debug_messenger(&vk);
  create_surface(&vk, window);
  pick_physical_device(&vk);
  create_logical_device(&vk);
  // VULKAN SECTION END

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
  }

  clean_up(&vk, window);
  return 0;
}
