#include "Buffer.h"
#include <stdexcept>

Buffer::Buffer(VkCommandPool& cmdPool, VkQueue& queue, VkDevice& logicalDevice, VkPhysicalDevice& physicalDevice)
: commandPool(cmdPool), queue(queue), logicalDevice(logicalDevice), physicalDevice(physicalDevice){}

void Buffer::initBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties){
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage; //this affects buffer offset/alignment/where data begins in buffer
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if(vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS){
        throw std::runtime_error("Failed to create buffer");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(logicalDevice, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;   //may have additional memory needs to store buffer in device memory (padding, extra info, etc.)
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties); //memoryTypeBits is a bit string representing which memory type indices are suitable for the buffer

    if(vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS){
        throw std::runtime_error("Failed to allocate vertex buffer memory");
    }
    vkBindBufferMemory(logicalDevice, buffer, bufferMemory, 0);
}

unsigned int Buffer::findMemoryType(unsigned int typeFilter, VkMemoryPropertyFlags properties){
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);                    //query physical device for memory types it can allocate from

        for(unsigned int i = 0; i < memProperties.memoryTypeCount; i++){                        //iterates through all memory types
            if((typeFilter & (1 << i))                                                          //checks if type filter says memory type is valid
                && (memProperties.memoryTypes[i].propertyFlags & properties) == properties){    //checks if memory type supports all properties
                return i;                                                                       //returns first memory type supporting device, buffer, and given properties
            }
        }

        throw std::runtime_error("Failed to find suitable memory type");
    }

void Buffer::copyFrom(Buffer& other, VkDeviceSize size){
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, other.getBuffer(), buffer, 1, &copyRegion);
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);
    vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);
}

void Buffer::destroyBuffer(){
    vkDestroyBuffer(logicalDevice, buffer, nullptr);
    vkFreeMemory(logicalDevice, bufferMemory, nullptr);
}