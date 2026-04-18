#include "Entity.h"

Entity::Entity(std::vector<Vertex> vData, std::vector<uint16_t> iData, VkCommandPool& cmdPool, VkQueue& queue, VkDevice& logicalDevice, VkPhysicalDevice& physicalDevice)
: mesh(vData, iData, cmdPool, queue, logicalDevice, physicalDevice){}