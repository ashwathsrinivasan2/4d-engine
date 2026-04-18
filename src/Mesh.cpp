#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vData, std::vector<uint16_t> iData, VkCommandPool& cmdPool, VkQueue& queue, VkDevice& logicalDevice, VkPhysicalDevice& physicalDevice)
: vertexData(vData), indexData(iData), cmdPool(cmdPool), queue(queue), logicalDevice(logicalDevice), physicalDevice(physicalDevice),
  vertexBuffer(cmdPool, queue, logicalDevice, physicalDevice), indexBuffer(cmdPool, queue, logicalDevice, physicalDevice){
    populateVertexBuffer();
    populateIndexBuffer();
}

void Mesh::populateVertexBuffer(){
    VkDeviceSize bufferSize = sizeof(vertexData[0]) * vertexData.size();
    Buffer stagingBuffer(cmdPool, queue, logicalDevice, physicalDevice);
    stagingBuffer.initBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vertexBuffer.initBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    stagingBuffer.populateBufferMemory(vertexData.data(), bufferSize);
    vertexBuffer.copyFrom(stagingBuffer, bufferSize);
    stagingBuffer.destroyBuffer();
}

void Mesh::populateIndexBuffer(){
    VkDeviceSize bufferSize = sizeof(indexData[0]) * indexData.size();
    Buffer stagingBuffer(cmdPool, queue, logicalDevice, physicalDevice);
    stagingBuffer.initBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    indexBuffer.initBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    stagingBuffer.populateBufferMemory(indexData.data(), bufferSize);
    indexBuffer.copyFrom(stagingBuffer, bufferSize);
    stagingBuffer.destroyBuffer();
}

void Mesh::destroyBuffers(){
    indexBuffer.destroyBuffer();
    vertexBuffer.destroyBuffer();
}

