#ifndef SCENE_H
#define SCENE_H

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_macos.h>

#include "Entity.h"
#include "Camera.h"
#include <vector>

class Scene{
    std::vector<Entity> entities;
    Camera cam;

    //for buffer creation
    VkCommandPool cmdPool;
    VkQueue queue;
    VkDevice logicalDevice;
    VkPhysicalDevice physicalDevice;

    struct HyperVertex{
        glm::vec4 pos;
        glm::vec3 color;
        glm::vec4 normal;
    };

    public:
    Scene();
    void initialize(VkCommandPool&, VkQueue&, VkDevice&, VkPhysicalDevice&);
    int createEntity(std::vector<Vertex>, std::vector<uint16_t>);
    std::vector<Entity>& getEntities(){return entities;}
    Camera& getCamera(){return cam;}

    void translate(int entityIndex, glm::vec3 translation) {entities[entityIndex].translate(translation);}
    void scale(int entityIndex, glm::vec3 scaleVec) {entities[entityIndex].scale(scaleVec);}
    void rotate(int entityIndex, glm::vec3 axis, float radians) {entities[entityIndex].rotate(axis, radians);}

    //Entity Factory Functions
    int Cube(glm::vec3);
    int Cylinder(glm::vec3);
    int Sphere(glm::vec3);
    int Tesseract(glm::vec3);
};

#endif