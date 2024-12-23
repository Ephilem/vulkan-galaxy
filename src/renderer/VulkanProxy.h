//
// Created by raph on 11/11/24.
//

#ifndef VULKANPROXY_H
#define VULKANPROXY_H
#include <vulkan/vulkan.h>

namespace VulkanProxy {
    VkResult CreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger);

    void DestroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator);
}

#endif //VULKANPROXY_H
