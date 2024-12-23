
#include "Buffer.h"

#include <cstring>
#include <stdexcept>

#include "VulkanContext.h"

Buffer::Buffer(VulkanContext& context,
               VkDeviceSize size,
               VkBufferUsageFlags usage,
               VkMemoryPropertyFlags properties)
    : context(context)
    , buffer(VK_NULL_HANDLE)
    , memory(VK_NULL_HANDLE)
    , bufferSize(size) {

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(context.getDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create buffer");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(context.getDevice(), buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(context.getDevice(), &allocInfo, nullptr, &memory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate buffer memory");
    }

    vkBindBufferMemory(context.getDevice(), buffer, memory, 0);
}

Buffer::~Buffer() {
    if (buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(context.getDevice(), buffer, nullptr);
    }
    if (memory != VK_NULL_HANDLE) {
        vkFreeMemory(context.getDevice(), memory, nullptr);
    }
}

void Buffer::copyFrom(const void* data, VkDeviceSize size) {
    void* mapped;
    vkMapMemory(context.getDevice(), memory, 0, size, 0, &mapped);
    memcpy(mapped, data, size);
    vkUnmapMemory(context.getDevice(), memory);
}

void Buffer::bindAsVertex(VkCommandBuffer commandBuffer, VkDeviceSize offset) const {
    VkBuffer buffers[] = {buffer};
    VkDeviceSize offsets[] = {offset};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
}

void Buffer::bindAsIndex(VkCommandBuffer commandBuffer, VkDeviceSize offset = 0) const {
    vkCmdBindIndexBuffer(commandBuffer, buffer, offset, VK_INDEX_TYPE_UINT16);
}

uint32_t Buffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(context.getPhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type");
}