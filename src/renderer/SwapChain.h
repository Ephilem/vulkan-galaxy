//
// Created by raph on 13/11/24.
//

#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include "../core/Logger.h"

class RenderPass;
class VulkanContext;

class SwapChain {
public:
    SwapChain(VulkanContext& context);
    ~SwapChain();

    void recreate();
    VkExtent2D getExtent() const { return extent; }
    VkFormat getImageFormat() const { return imageFormat; }
    VkRenderPass getRenderPass() const { return renderPass; }
    const std::vector<VkImageView>& getImageViews() const { return imageViews; }
    const std::vector<VkFramebuffer>& getFramebuffers() const { return framebuffers; }

    // Add these render pass methods
    void beginRenderPass(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer) const;

    static void endRenderPass(VkCommandBuffer commandBuffer);

    VkSwapchainKHR swapChain{};

private:
    void create();
    void cleanup();
    void createImageViews();
    void createRenderPass();
    void createFramebuffers();

    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    VulkanContext& context;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    std::vector<VkFramebuffer> framebuffers;
    VkRenderPass renderPass{};
    VkFormat imageFormat;
    VkExtent2D extent{};
    Logger logger;
};



#endif //SWAPCHAIN_H
