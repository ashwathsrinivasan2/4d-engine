#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_macos.h>

#include "Buffer.h"
#include "Vertex.h"
#include <vector>
#include <iostream>

class Mesh{
    Buffer vertexBuffer;
    Buffer indexBuffer;
    std::vector<Vertex> vertexData;
    std::vector<uint16_t> indexData;

    //for buffer creation
    VkCommandPool cmdPool;
    VkQueue queue;
    VkDevice logicalDevice;
    VkPhysicalDevice physicalDevice;

    void populateVertexBuffer();
    void populateIndexBuffer();


    public:
    Mesh(std::vector<Vertex>, std::vector<uint16_t>, VkCommandPool&, VkQueue&, VkDevice&, VkPhysicalDevice&);
    Buffer getVertexBuffer(){return vertexBuffer;}
    Buffer getIndexBuffer(){return indexBuffer;}
    unsigned getNumVertices(){return indexData.size();}
    void destroyBuffers();
};

#endif