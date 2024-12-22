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
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR *formats;
    uint32_t format_count, present_mode_count;
    VkPresentModeKHR *present_modes;
} swap_chain_support_details_t;

typedef struct
{
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkSurfaceKHR surface;
} vk_t;

typedef struct
{
    uint32_t value;
    bool has_value;
} optional_uint32t_t;
typedef struct
{
    optional_uint32t_t graphics_family;
    optional_uint32t_t present_family;
} queue_familiy_indices_t;

vk_t create_instance();

void create_surface(vk_t *vk, GLFWwindow *window);

void pick_physical_device(vk_t *vk);

void setup_debug_messenger(vk_t *vk);

void create_logical_device(vk_t *vk);

void create_swap_chain(vk_t *vk, GLFWwindow *window);

int clamp(int value, int min, int max);

swap_chain_support_details_t query_swap_chain_support(VkPhysicalDevice device, VkSurfaceKHR surface);

VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR *capabilities, GLFWwindow *window);

VkSurfaceFormatKHR choose_swap_surface_format(VkSurfaceFormatKHR *available_formats, uint32_t format_count);

VkPresentModeKHR choose_swap_present_mode(VkPresentModeKHR *available_present_modes, uint32_t present_mode_count);

bool check_device_extension_support(VkPhysicalDevice device);

bool is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface);

queue_familiy_indices_t find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface);
bool is_complete(queue_familiy_indices_t *indices);

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
