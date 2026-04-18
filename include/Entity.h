#ifndef ENTITY_H
#define ENTITY_H

#include "Mesh.h"
#include "Transform.h"
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_macos.h>

class Entity{
    Mesh mesh;
    Transform transform;

    //for buffer creation
    VkCommandPool cmdPool;
    VkQueue queue;
    VkDevice logicalDevice;
    VkPhysicalDevice physicalDevice;

    public:
    Entity(std::vector<Vertex>, std::vector<uint16_t>, VkCommandPool&, VkQueue&, VkDevice&, VkPhysicalDevice&);

    //mesh functions
    Buffer getVertexBuffer(){return mesh.getVertexBuffer();}
    Buffer getIndexBuffer(){return mesh.getIndexBuffer();}
    unsigned getNumVertices(){return mesh.getNumVertices();}
    void destroyBuffers(){mesh.destroyBuffers();}

    //transform functions
    void translate(glm::vec3 translation){transform.translate(translation);}
    void scale(glm::vec3 scaleVec){transform.scale(scaleVec);}
    void rotate(glm::vec3 axis, float radians){transform.rotate(axis, radians);}
    glm::mat4 getModelMatrix(){return transform.getModelMatrix();}
};

#endif