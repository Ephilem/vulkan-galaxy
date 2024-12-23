//
// Created by raph on 13/11/24.
//

#ifndef VULKANCONTEXT_H
#define VULKANCONTEXT_H

#include <vulkan/vulkan.h>
#include <memory>
#include "../core/Window.h"
#include <optional>
#include <vector>

#include "CommandManager.h"
#include "SwapChain.h"
#include "../core/Logger.h"


// Store queue family indices, used to check if a device supports the required queues
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

// Swap chain support details structure
struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

/**
 * Class that manage the Vulkan context, including the instance, physical and logical devices, queues, swap chain and command buffers
 */
class VulkanContext {
public:
    explicit VulkanContext(Window& window);
    ~VulkanContext();

    VulkanContext(const VulkanContext&) = delete;
    VulkanContext& operator=(const VulkanContext&) = delete;

    void initialize();
    void waitIdle() const;

    VkDevice getDevice() const { return device; }
    VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
    VkInstance getInstance() const { return instance; }
    VkQueue getGraphicsQueue() const { return graphicsQueue; }
    VkQueue getPresentQueue() const { return presentQueue; }
    VkSurfaceKHR getSurface() const { return surface; }
    SwapChain& getSwapChain() const { return *swapChain; }
    CommandManager& getCommandManager() const { return *commandManager; }
    Window& getWindow() const { return window; }

    /**
     * Get the queue family indices for a given physical device. This also can be used to check if the device
     * supports the required queues, and if the graphics and present queues are different or not
     * @param device the device to query
     * @return QueueFamilyIndices structure with the queue family indices. Use isComplete() to check if the device supports the required queues
     */
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;

    /**
     * Query information about swap chain support for a given device
     * @param device the device to query
     * @return SwapChainSupportDetails structure with the swap chain support information
     */
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) const;

private:
    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();

    /**
     * Check if all device extensions in deviceExtensions are supported by the device
     * @param device the device to check
     * @return true if all extensions are supported, false otherwise
     */
    bool checkDeviceExtensionSupport(VkPhysicalDevice device) const;

    /**
     * Check if all layer names in validationLayers are supported by the Vulkan instance
     * @return true if all layers are supported, false otherwise
     */
    bool checkValidationLayerSupport() const;

    /**
     * Get all required extensions for the Vulkan instance to interface with the window system and validation layers if enabled
     * @return a vector of const char* with the required extensions
     */
    std::vector<const char*> getRequiredExtensions() const;

    /**
     * Populate the debug messenger create info structure with the required information
     * @param createInfo the structure to populate
     */
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) ;

    /**
     * Test if all functionnalities required by the application are supported by the device
     * @param device the device to test
     * @return true if the device has extensions, queues and swap chain support, false otherwise
     */
    bool isDeviceSuitable(VkPhysicalDevice device) const;

    /**
     * Give a score to a physical device based on its properties. Used in the device selection process
     * @param device the device to rate
     * @return a positive score if the device is suitable, 0 otherwise
     */
    static int rateDeviceSuitability(VkPhysicalDevice device);

    /**
     * Callback function for Vulkan debug messages
     * @param messageSeverity the severity of the message
     * @param messageType type of the message
     * @param pCallbackData  the message data
     * @param pUserData user defined data. Here, a pointer to the VulkanContext instance to Vulkan can use the associated logger
     * @return VK_FALSE
     */
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    Window& window;
    Logger logger;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSurfaceKHR surface;

    std::unique_ptr<SwapChain> swapChain;
    std::unique_ptr<CommandManager> commandManager;


    // Configuration

    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    #ifdef NDEBUG
        static constexpr bool enableValidationLayers = false;
    #else
        static constexpr bool enableValidationLayers = true;
    #endif
};


#endif //VULKANCONTEXT_H
