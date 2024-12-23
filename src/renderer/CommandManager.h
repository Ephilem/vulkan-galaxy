//
// Created by raph on 14/11/24.
//

#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <vulkan/vulkan.h>
#include <vector>
#include "../core/Logger.h"

class VulkanContext;

class CommandManager {
public:
    explicit CommandManager(VulkanContext& context);
    ~CommandManager();

    CommandManager(const CommandManager&) = delete;
    CommandManager& operator=(const CommandManager&) = delete;

    VkCommandBuffer beginSingleTimeCommands() const;
    void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;

    VkCommandBuffer getCurrentBuffer() { return commandBuffers[currentFrame]; }
    void resetCurrentBuffer() const;
    void createCommandBuffers(uint32_t count);
    void freeCommandBuffers();
    void setCurrentFrame(uint32_t frame) { currentFrame = frame; }


private:
    void createCommandPool();

    VulkanContext& context;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    uint32_t currentFrame{0};
    Logger logger;
};

#endif //COMMANDMANAGER_H
