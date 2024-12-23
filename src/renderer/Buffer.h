//
// Created by raph on 21/12/24.
//

#ifndef BUFFER_H
#define BUFFER_H
#include <vulkan/vulkan.h>

class VulkanContext;

class Buffer {
public:
    Buffer(VulkanContext &context,
           VkDeviceSize size,
           VkBufferUsageFlags usage,
           VkMemoryPropertyFlags properties);

    ~Buffer();

    void copyFrom(const void* data, VkDeviceSize size);
    void bindAsVertex(VkCommandBuffer commandBuffer, VkDeviceSize offset = 0) const;

    void bindAsIndex(VkCommandBuffer commandBuffer, VkDeviceSize offset) const;

    VkBuffer getBuffer() const { return buffer; }
    VkDeviceMemory getMemory() const { return memory; }
    VkDeviceSize getSize() const { return bufferSize; }

private:
    VulkanContext& context;
    VkBuffer buffer;
    VkDeviceMemory memory;
    VkDeviceSize bufferSize;

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};

#endif //BUFFER_H
