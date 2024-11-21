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

  setup_debug_messenger(&vk);
  return vk;
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
    if (is_device_suitable(devices[i]))
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
  queue_familiy_indices_t indices = find_queue_families(vk->physical_device);
  VkDeviceQueueCreateInfo queue_create_info = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .queueFamilyIndex = indices.graphics_family,
      .queueCount = 1,
      .pQueuePriorities = (float[]){1.0},
  };

  VkPhysicalDeviceFeatures device_features = {};

  VkDeviceCreateInfo device_info = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pQueueCreateInfos = &queue_create_info,
      .queueCreateInfoCount = 1,
      .pEnabledFeatures = &device_features,
  };

  // TODO: section creating logical device in khoronos documentation
  // https://docs.vulkan.org/tutorial/latest/03_Drawing_a_triangle/00_Setup/04_Logical_device_and_queues.html#_creating_the_logical_device
  // sleepy :)
}

bool is_device_suitable(VkPhysicalDevice device)
{
  queue_familiy_indices_t indices = find_queue_families(device);
  return indices.graphics_family >= 0;
}

queue_familiy_indices_t find_queue_families(VkPhysicalDevice device)
{
  queue_familiy_indices_t indices;
  uint32_t queue_family_count = 0;

  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);

  VkQueueFamilyProperties queue_families[queue_family_count];

  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);

  uint32_t i = 0;
  for (i = 0; i < queue_family_count; ++i)
  {
    if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
    {
      indices.graphics_family = i;
      break;
    }

    if (indices.graphics_family >= 0)
    {
      break;
    }
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
