//
// Created by raph on 14/11/24.
//

#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H
#include <vulkan/vulkan.h>
#include <vector>
#include "../core/Logger.h"

class VulkanContext;

class Synchronization {
public:
    explicit Synchronization(VulkanContext& context, uint32_t maxFramesInFlight);
    ~Synchronization();

    // Delete copy constructor and operator
    Synchronization(const Synchronization&) = delete;
    Synchronization& operator=(const Synchronization&) = delete;

    void waitForFence(uint32_t frameIndex);
    void resetFence(uint32_t frameIndex);

    // Getters for synchronization objects
    VkSemaphore getImageAvailableSemaphore(uint32_t frameIndex) const {
        return imageAvailableSemaphores[frameIndex];
    }

    VkSemaphore getRenderFinishedSemaphore(uint32_t frameIndex) const {
        return renderFinishedSemaphores[frameIndex];
    }

    VkFence getFence(uint32_t frameIndex) const {
        return inFlightFences[frameIndex];
    }

private:
    void createSyncObjects();

    VulkanContext& context;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t maxFramesInFlight;
    Logger logger;
};



#endif //SYNCHRONIZATION_H
