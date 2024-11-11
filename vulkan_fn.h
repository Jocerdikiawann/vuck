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

typedef struct
{
  VkInstance instance;
} vk_t;

vk_t create_instance();
bool check_validation_layer_support();
const char **get_required_extensions();

#endif // !VULKAN_FN_H
