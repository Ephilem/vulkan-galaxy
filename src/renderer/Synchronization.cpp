//
// Created by raph on 14/11/24.
//

#include "Synchronization.h"
#include "VulkanContext.h"

Synchronization::Synchronization(VulkanContext& context, uint32_t maxFramesInFlight)
    : context(context)
      , maxFramesInFlight(maxFramesInFlight), logger("Synchronization") {
    createSyncObjects();
}

Synchronization::~Synchronization() {
    auto device = context.getDevice();

    for (size_t i = 0; i < maxFramesInFlight; i++) {
        vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device, inFlightFences[i], nullptr);
    }
}

void Synchronization::createSyncObjects() {
    imageAvailableSemaphores.resize(maxFramesInFlight);
    renderFinishedSemaphores.resize(maxFramesInFlight);
    inFlightFences.resize(maxFramesInFlight);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;  // Create fence in signaled state

    for (size_t i = 0; i < maxFramesInFlight; i++) {
        if (vkCreateSemaphore(context.getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(context.getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(context.getDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
    }
}

void Synchronization::waitForFence(uint32_t frameIndex) {
    vkWaitForFences(context.getDevice(), 1, &inFlightFences[frameIndex], VK_TRUE, UINT64_MAX);
}

void Synchronization::resetFence(uint32_t frameIndex) {
    vkResetFences(context.getDevice(), 1, &inFlightFences[frameIndex]);
}
