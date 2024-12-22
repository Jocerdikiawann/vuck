#include "vulkan_fn.h"

// TODO:
//  1. Create swap chain :  https://docs.vulkan.org/tutorial/latest/03_Drawing_a_triangle/01_Presentation/01_Swap_chain.html#_creating_the_swap_chain

int size_of_validation_layers = 1;
const char *validation_layers[] = {
    "VK_LAYER_KHRONOS_validation",
};

int size_of_device_extensions = 1;
const char *device_extensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
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
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = "Hello Triangle",
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = "No Engine",
      .engineVersion = VK_MAKE_VERSION(1, 0, 0),
      .apiVersion = VK_API_VERSION_1_0,
  };

  VkInstanceCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &app_info,
      .enabledLayerCount = 0,
  };

  uint32_t extension_count = 0;
  const char **debug_glfw_extensions = glfwGetRequiredInstanceExtensions(&extension_count);
  const char *extensions[extension_count + 1];
  for (size_t i = 0; i < extension_count; ++i)
  {
    extensions[i] = debug_glfw_extensions[i];
  }

  if (ENABLE_VALIDATION_LAYERS)
  {
    extensions[extension_count] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    extension_count++;
  }

  create_info.enabledExtensionCount = extension_count;
  create_info.ppEnabledExtensionNames = extensions;

  VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {};
  if (ENABLE_VALIDATION_LAYERS)
  {
    create_info.enabledLayerCount = size_of_validation_layers;
    create_info.ppEnabledLayerNames = validation_layers;
    debug_create_info = populate_debug_messenger_create_info();
    create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debug_create_info;
  }
  else
  {
    create_info.enabledLayerCount = 0;
    create_info.pNext = NULL;
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

void create_surface(vk_t *vk, GLFWwindow *window)
{
  if (glfwCreateWindowSurface(vk->instance, window, NULL, &vk->surface) != VK_SUCCESS)
  {
    printf("Failed to create window surface\n");
    exit(0);
  }
}

void pick_physical_device(vk_t *vk)
{
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(vk->instance, &device_count, NULL);

  if (device_count == 0)
  {
    printf("Failed to find GPUs with Vulkan support\n");
    exit(0);
  }

  VkPhysicalDevice devices[device_count];
  vkEnumeratePhysicalDevices(vk->instance, &device_count, devices);

  for (int i = 0; i < device_count; ++i)
  {
    if (is_device_suitable(devices[i], vk->surface))
    {
      vk->physical_device = devices[i];
      break;
    }
  }

  if (vk->physical_device == VK_NULL_HANDLE)
  {
    printf("Failed to find a suitable GPU\n");
    exit(0);
  }
}

void create_logical_device(vk_t *vk)
{
  queue_familiy_indices_t indices = find_queue_families(vk->physical_device, vk->surface);
  int size_queue_create_info = 2;
  VkDeviceQueueCreateInfo queue_create_infos[size_queue_create_info];
  uint32_t unique_queue_families[] = {indices.graphics_family.value, indices.present_family.value};
  float queue_priority = 1.0;

  for (size_t i = 0; i < size_queue_create_info; ++i)
  {
    VkDeviceQueueCreateInfo queue_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = unique_queue_families[i],
        .queueCount = 1,
        .pQueuePriorities = &queue_priority,
    };

    queue_create_infos[i] = queue_create_info;
  }

  VkPhysicalDeviceFeatures device_features = {};

  VkDeviceCreateInfo device_info = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .queueCreateInfoCount = size_queue_create_info,
      .pQueueCreateInfos = queue_create_infos,
      .pEnabledFeatures = &device_features,
      .enabledExtensionCount = size_of_device_extensions,
      .ppEnabledLayerNames = device_extensions,
  };

  if (ENABLE_VALIDATION_LAYERS)
  {
    device_info.enabledLayerCount = size_of_validation_layers;
    device_info.ppEnabledLayerNames = validation_layers;
  }
  else
  {
    device_info.enabledLayerCount = 0;
  }

  if (vkCreateDevice(vk->physical_device, &device_info, VK_NULL_HANDLE, &vk->device) != VK_SUCCESS)
  {
    printf("Failed to create logical device\n");
    exit(0);
  }

  vkGetDeviceQueue(vk->device, indices.graphics_family.value, 0, &vk->graphics_queue);
  vkGetDeviceQueue(vk->device, indices.present_family.value, 0, &vk->present_queue);
}

void create_swap_chain(vk_t *vk, GLFWwindow *window)
{
  swap_chain_support_details_t swap_chain_support = query_swap_chain_support(vk->physical_device, vk->surface);
  VkSurfaceFormatKHR surface_format = choose_swap_surface_format(swap_chain_support.formats, swap_chain_support.format_count);
  VkPresentModeKHR present_mode = choose_swap_present_mode(swap_chain_support.present_modes, swap_chain_support.present_mode_count);
  VkExtent2D extent = choose_swap_extent(&swap_chain_support.capabilities, window);
}

bool check_device_extension_support(VkPhysicalDevice device)
{
  uint32_t extension_count;
  vkEnumerateDeviceExtensionProperties(device, VK_NULL_HANDLE, &extension_count, VK_NULL_HANDLE);

  VkExtensionProperties available_extensions[extension_count];

  vkEnumerateDeviceExtensionProperties(device, VK_NULL_HANDLE, &extension_count, available_extensions);

  // membandingkan apakah device extension yang diminta ada di available_extension
  for (size_t i = 0; i < size_of_device_extensions; ++i)
  {
    bool extension_found = false;
    for (size_t j = 0; j < extension_count; ++j)
    {
      if (strcmp(device_extensions[i], available_extensions[j].extensionName) == 0)
      {
        extension_found = true;
        break;
      }
    }
    if (!extension_found)
    {
      return false;
    }
  }

  return true;
}

VkSurfaceFormatKHR choose_swap_surface_format(VkSurfaceFormatKHR *available_formats, uint32_t format_count)
{
  // Each VkSurfaceFormatKHR entry contains a format and a colorSpace member.
  for (size_t i = 0; i < format_count; ++i)
  {
    if (available_formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && available_formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
    {
      return available_formats[i];
    }
  }
  return available_formats[0];
};

VkPresentModeKHR choose_swap_present_mode(VkPresentModeKHR *available_present_modes, uint32_t present_mode_count)
{
  for (size_t i = 0; i < present_mode_count; ++i)
  {
    if (available_present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
    {
      return available_present_modes[i];
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

int clamp(int value, int min, int max)
{
  if (value < min)
    return min;
  if (value > max)
    return max;
  return value;
}

VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR *capabilities, GLFWwindow *window)
{
  if (capabilities->currentExtent.width != UINT32_MAX)
  {
    return capabilities->currentExtent;
  }
  else
  {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D actual_extent = {
        .width = width,
        .height = height,
    };

    actual_extent.height = clamp(actual_extent.height, capabilities->minImageExtent.height, capabilities->maxImageExtent.height);
    actual_extent.width = clamp(actual_extent.width, capabilities->minImageExtent.width, capabilities->maxImageExtent.width);

    return actual_extent;
  }
}

swap_chain_support_details_t query_swap_chain_support(VkPhysicalDevice device, VkSurfaceKHR surface)
{
  swap_chain_support_details_t details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

  uint32_t format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, NULL);

  if (format_count != 0)
  {
    details.formats[format_count];
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.formats);
  }

  uint32_t present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, NULL);
  if (present_mode_count != 0)
  {
    details.present_modes[present_mode_count];
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, details.present_modes);
  }

  details.format_count = format_count;
  details.present_mode_count = present_mode_count;

  return details;
}

bool is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface)
{
  queue_familiy_indices_t indices = find_queue_families(device, surface);

  bool extension_supported = check_device_extension_support(device);

  bool swap_chain_adequate = false;
  if (extension_supported)
  {
    swap_chain_support_details_t swap_chain_support = query_swap_chain_support(device, surface);
    swap_chain_adequate = swap_chain_support.formats != NULL && swap_chain_support.present_modes != NULL;
  }

  return is_complete(&indices) && extension_supported && swap_chain_adequate;
}

bool is_complete(queue_familiy_indices_t *indices)
{
  return indices->graphics_family.has_value && indices->present_family.has_value;
}

queue_familiy_indices_t find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface)
{
  queue_familiy_indices_t indices = {0};
  uint32_t queue_family_count = 0;

  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);

  VkQueueFamilyProperties queue_families[queue_family_count];

  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);
  for (uint32_t i = 0; i < queue_family_count; ++i)
  {
    if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
    {
      indices.graphics_family.value = i;
      indices.graphics_family.has_value = true;
    }

    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);
    if (present_support)
    {
      indices.present_family.value = i;
      indices.present_family.has_value = true;
    }

    // in
    //  if(is_complete(&indices))
    //  break;
  }
  return indices;
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
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = debug_callback,
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

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data,
    void *p_user_data)
{
  if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    log_message("WARNING", p_callback_data->pMessage);
  else if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    log_message("ERROR", p_callback_data->pMessage);
  else if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    log_message("INFO", p_callback_data->pMessage);
  else if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
    log_message("VERBOSE", p_callback_data->pMessage);

  return VK_FALSE;
}

void log_message(const char *severity, const char *message)
{
  printf("[%s] Validation layer: %s\n", severity, message);
}
