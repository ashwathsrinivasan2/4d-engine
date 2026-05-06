#ifndef SCENE_H
#define SCENE_H

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_macos.h>

#include "Rotor.h"
#include "Camera.h"
#include <vector>
#include <iostream>

class Scene{

    struct Vertex{
        glm::vec4 pos;
        glm::vec4 color;
        glm::vec4 normal;
        glm::vec4 texCoord;
        glm::uvec4 instanceID = glm::uvec4(0, 0, 0, 0);
        
        Vertex(glm::vec4 pos, glm::vec3 color, glm::vec4 normal, glm::vec3 texCoord)
            : pos(pos), color(glm::vec4(color, 1.f)), normal(normal), texCoord(glm::vec4(texCoord, 1.f)) {
        };
    };

    struct Instance {
        glm::vec4 currTranslate = glm::vec4(0.f);
        glm::vec4 currScale = glm::vec4(1.f);
        Rotor currRotation;
    };

    struct ConvertedInstance {
        glm::mat4 model;
        glm::vec4 modelTranslate;
    };

    std::vector<Vertex> vertices;
    std::vector<Instance> instances;

    Camera cam;

    std::vector<ConvertedInstance> convertInstances();

    void printVec(std::string name, glm::vec4 vec) {
        std::cout << name << ": {";
        for (int i = 0; i < 4; i++) {
            std::cout << vec[i];
            if (i != 4) std::cout << " ";
        }
        std::cout << "}";
    }

    glm::vec4 crossProduct4D(glm::vec4, glm::vec4, glm::vec4);
    void correctWindingOrder(std::vector<Vertex>& vertexData, glm::vec4 center = glm::vec4(0.f));

    public:
    Scene();
    int createEntity(std::vector<Vertex>);

    size_t getVertexBufferSize() { return sizeof(Vertex) * vertices.size(); }
    size_t getInstanceBufferSize() { return sizeof(ConvertedInstance) * instances.size(); }

    std::vector<Vertex> getVertexData() { return vertices; }
    std::vector<ConvertedInstance> getInstanceData() { return convertInstances(); }
    Camera& getCamera(){return cam;}

    
    void translate(int, glm::vec4);
    void scale(int , glm::vec4);
    void rotate(int, int, int, int, int, float);

    int getNumTetrahedrons() { return vertices.size() / 4; }
    

    //Entity Factory Functions
    //int Cube(glm::vec3);
    //int Cylinder(glm::vec3);
    //int Sphere(glm::vec3);

    int Tesseract(glm::vec3);
};

#endif