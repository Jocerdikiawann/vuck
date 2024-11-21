#ifndef VULKAN_FN_H
#define VULKAN_FN_H

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define ENABLE_VALIDATION_LAYERS true

typedef struct
{
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkPhysicalDevice physical_device;
} vk_t;

typedef struct
{
    uint32_t graphics_family;
} queue_familiy_indices_t;

vk_t create_instance();

void pick_physical_device(vk_t *vk);

void setup_debug_messenger(vk_t *vk);

bool is_device_suitable(VkPhysicalDevice device);

queue_familiy_indices_t find_queue_families(VkPhysicalDevice device);

VkResult create_debug_utils_messenger_ext(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *p_create_info,
    const VkAllocationCallbacks *p_allocator,
    VkDebugUtilsMessengerEXT *p_debug_messenger);

VkDebugUtilsMessengerCreateInfoEXT populate_debug_messenger_create_info();

void destroy_debug_utils_messenger_ext(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debug_messenger,
    const VkAllocationCallbacks *p_allocator);

bool check_validation_layer_support();

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data,
    void *p_user_data);

void log_message(const char *severity, const char *message);

#endif // !VULKAN_FN_H
