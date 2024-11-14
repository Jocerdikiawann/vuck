#include "vulkan_fn.h"

int size_of_validation_layers = 1;
const char *validation_layers[] = {
    "VK_LAYER_KHRONOS_validation",
};

vk_t create_instance()
{
  if (ENABLE_VALIDATION_LAYERS && !check_validation_layer_support())
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
      .enabledLayerCount = 0,
  };

  VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {};
  if (ENABLE_VALIDATION_LAYERS)
  {
    extensions_t extensions;
    get_required_extensions(&extensions);
    create_info.enabledLayerCount = extensions.count;
    create_info.ppEnabledLayerNames = extensions.extension;

    debug_create_info = populate_debug_messenger_create_info();
    create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debug_create_info;
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

  setup_debug_messenger(&vk);
  return vk;
}

VkResult create_debug_utils_messenger_ext(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *p_create_info,
    const VkAllocationCallbacks *p_allocator,
    VkDebugUtilsMessengerEXT *p_debug_messenger)
{
  PFN_vkCreateDebugUtilsMessengerEXT func =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
          instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != NULL)
  {
    return func(instance, p_create_info, p_allocator, p_debug_messenger);
  }
  else
  {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
};

void destroy_debug_utils_messenger_ext(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debug_messenger,
    const VkAllocationCallbacks *p_allocator)
{
  PFN_vkDestroyDebugUtilsMessengerEXT func =
      (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
          instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != NULL)
  {
    func(instance, debug_messenger, p_allocator);
  }
};

void setup_debug_messenger(vk_t *vk)
{
  if (!ENABLE_VALIDATION_LAYERS)
    return;

  VkDebugUtilsMessengerCreateInfoEXT create_info = populate_debug_messenger_create_info();

  if (create_debug_utils_messenger_ext(vk->instance, &create_info, NULL, &vk->debug_messenger) != VK_SUCCESS)
  {
    printf("Failed to set up debug messenger\n");
    exit(0);
  }
}

VkDebugUtilsMessengerCreateInfoEXT populate_debug_messenger_create_info()
{
  VkDebugUtilsMessengerCreateInfoEXT create_info = {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = debug_callback,
      .pUserData = VK_NULL_HANDLE,
  };
  return create_info;
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
  uint32_t extenstion_count = glfw_extension_count + 1;
  const char *extensions[extenstion_count];
  if (ENABLE_VALIDATION_LAYERS)
  {
    for (int i = 0; i < glfw_extension_count; ++i)
    {
      extensions[i] = glfw_extensions[i];
    }
    // set the last element to the debug extension
    extensions[extenstion_count] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    extension->count = extenstion_count;
  }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data,
    void *p_user_data)
{

  log_message("INFO", p_callback_data->pMessage);

  return VK_FALSE;
}

void log_message(const char *severity, const char *message)
{
  printf("[%s] Validation layer: %s\n", severity, message);
}
