#ifndef BUFFER_H
#define BUFFER_H

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_macos.h>
#include <string.h>


class Buffer{
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;

    VkCommandPool commandPool;
    VkQueue queue;
    VkDevice logicalDevice;
    VkPhysicalDevice physicalDevice;

    unsigned int findMemoryType(unsigned int typeFilter, VkMemoryPropertyFlags properties);

    public:

    Buffer(VkCommandPool&, VkQueue&, VkDevice&, VkPhysicalDevice&);
    void initBuffer(VkDeviceSize, VkBufferUsageFlags, VkMemoryPropertyFlags);
    void copyFrom(Buffer&, VkDeviceSize);

    template <typename T>
    void populateBufferMemory(T* data, VkDeviceSize size){
        void* tempData;
        vkMapMemory(logicalDevice, bufferMemory, 0, size, 0, &tempData); //syncs GPU memory (stagingBufferMemory) with CPU memory (data)
        memcpy(tempData, data, (unsigned long long)size);
        vkUnmapMemory(logicalDevice, bufferMemory);
    }

    VkBuffer getBuffer(){return buffer;}
    VkDeviceMemory getBufferMemory(){return bufferMemory;}

    void destroyBuffer();
};

#endif