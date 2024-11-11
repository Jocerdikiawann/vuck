#include "vulkan_fn.h"

int size_of_validation_layers = 1;
const char *validation_layers[] = {
    "VK_LAYER_KHRONOS_validation",
};

#ifndef DEBUG
const bool enable_validation_layers = false;
#else
const bool enable_validation_layers = true;
#endif

vk_t create_instance()
{
  if (enable_validation_layers && !check_validation_layer_support())
  {
    printf("Validation layers requested, but not available!\n");
    exit(0);
  }

  VkInstance instance;
  VkApplicationInfo app_info = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationName = "Hello Triangle",
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = "No Engine",
      .engineVersion = VK_MAKE_VERSION(1, 0, 0),
      .apiVersion = VK_API_VERSION_1_0,
  };

  uint32_t glfw_extension_count = 0;
  const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

  VkInstanceCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &app_info,
      .enabledExtensionCount = glfw_extension_count,
      .ppEnabledExtensionNames = glfw_extensions,
      .enabledLayerCount = 0};

  if (enable_validation_layers)
  {
    extensions_t *extensions;
    get_required_extensions(extensions);
    create_info.enabledLayerCount = extensions->count;
    create_info.ppEnabledLayerNames = extensions->extension;
  }
  else
  {
    create_info.enabledLayerCount = 0;
  }

  if (vkCreateInstance(&create_info, NULL, &instance) != VK_SUCCESS)
  {
    printf("Failed to create instance vulkan\n");
    exit(0);
  }

  vk_t vk = {
      .instance = instance,
  };
  return vk;
}

bool check_validation_layer_support()
{
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, NULL);

  VkLayerProperties available_layers[layer_count];
  vkEnumerateInstanceLayerProperties(&layer_count, available_layers);

  for (int i = 0; i < size_of_validation_layers; ++i)
  {
    bool layer_found = false;

    for (int j = 0; j < layer_count; ++j)
    {
      if (strcmp(validation_layers[i], available_layers[j].layerName) == 0)
      {
        layer_found = true;
        break;
      }
    }

    if (!layer_found)
    {
      return false;
    }
  }

  return true;
}

void get_required_extensions(extensions_t *extension)
{
  uint32_t glfw_extension_count = 0;
  const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
  if (enable_validation_layers)
  {
    for (int i = 0; i < glfw_extension_count; ++i)
    {
      extension->extension[i] = glfw_extensions[i];
    }
    extension->count = glfw_extension_count;
  }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data,
    void *p_user_data)
{
  printf("Validation layer: %s\n", p_callback_data->pMessage);
  return VK_FALSE;
}


