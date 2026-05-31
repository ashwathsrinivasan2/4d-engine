#include "Scene.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>
#include <iostream>

Scene::Scene(){

}


int Scene::createEntity(std::vector<Vertex> vData, bool invert, glm::vec4 center){

    if((vertices.size() + vData.size()) > MAX_VERTICES) {
        throw std::runtime_error("Not enough memory to store new entity");
    }
    correctWindingOrder(vData, invert, center);
    for (int i = 0; i < vData.size(); i++) {
        Vertex newVertex = vData[i];
        newVertex.instanceID.x = instances.size();
        vertices.push_back(newVertex);
    }

    
    Instance newInstance;
    instances.push_back(newInstance);

    return instances.size() - 1;
}

void Scene::updateFromParent(int index, glm::mat4& model, glm::vec4& modelTranslate) {

    glm::mat4 scale(1.f);
    for (int i = 0; i < 4; i++) {
        scale[i][i] = instances[index].currScale[i];
    }

    glm::mat4 pModel = scale * instances[index].currRotation.toMatrix();
    glm::vec4 pModelTranslate = instances[index].currTranslate;

    model = pModel * model;
    modelTranslate = pModel * modelTranslate + pModelTranslate;


    if (instances[index].parentID != -1) {
        updateFromParent(instances[index].parentID, model, modelTranslate);
    }
}

std::vector<Scene::ConvertedInstance> Scene::convertInstances() {
    std::vector<ConvertedInstance> converted;
    for (int i = 0; i < instances.size(); i++) {
        glm::mat4 model(1.f);
        glm::vec4 translate(0.f);
        updateFromParent(i, model, translate);
        converted.push_back({ model, translate });
    }
        
    return converted;
}

glm::vec4 Scene::crossProduct4D(glm::vec4 a, glm::vec4 b, glm::vec4 c) {

    glm::vec4 normal;
    normal.x = glm::determinant(glm::mat3(a.y, a.z, a.w, b.y, b.z, b.w, c.y, c.z, c.w));
    normal.y = -glm::determinant(glm::mat3(a.x, a.z, a.w, b.x, b.z, b.w, c.x, c.z, c.w));
    normal.z = glm::determinant(glm::mat3(a.x, a.y, a.w, b.x, b.y, b.w, c.x, c.y, c.w));
    normal.w = -glm::determinant(glm::mat3(a.x, a.y, a.z, b.x, b.y, b.z, c.x, c.y, c.z));
    return normal;
}

void Scene::correctWindingOrder(std::vector<Vertex>& vertexData, bool invert, glm::vec4 center) {
    for (int i = 0; i < vertexData.size(); i += 4) {
        glm::vec4 vA = vertexData[i].pos - vertexData[i + 1].pos;
        glm::vec4 vB = vertexData[i].pos -vertexData[i + 2].pos;
        glm::vec4 vC = vertexData[i].pos - vertexData[i + 3].pos;
        glm::vec4 normal = crossProduct4D(vA, vB, vC);

        glm::vec4 cellCenter = (vertexData[i].pos + vertexData[i + 1].pos + vertexData[i + 2].pos + vertexData[i + 3].pos) / 4.f;
        glm::vec4 outwards = cellCenter - center;

        if ((!invert && glm::dot(outwards, normal) < 0) || (invert && glm::dot(outwards, normal) > 0)){
            Vertex temp = vertexData[i];
            vertexData[i] = vertexData[i + 1];
            vertexData[i + 1] = temp;
        }
    }
}

void Scene::translate(int entityIndex, glm::vec4 translation)
{
    instances[entityIndex].currTranslate += translation;
}

void Scene::scale(int entityIndex, glm::vec4 scale)
{
    instances[entityIndex].currScale *= scale;
}

void Scene::rotate(int entityIndex, int planeID, float radians) {
    Rotor newRotation(planeID, radians);
    instances[entityIndex].currRotation.rotate(newRotation);
}

int Scene::groupEntities(std::vector<int> entities) {
    int parentID = instances.size();

    //default, group entire scene
    if (entities.empty()) {
        for (int i = 0; i < instances.size(); i++) {
            entities.push_back(i);
        }
    }

    /*
    for (int i = 0; i < entities.size(); i++) {
        std::cout << entities[i] << " ";
    }
    std::cout << std::endl;
    */

    for (int i = 0; i < entities.size(); i++) {
        int childID = entities[i];
        if (instances[childID].parentID != -1) {
            throw std::runtime_error("At least one of given entities is already part of a group.");
        }
    }

    for (int i = 0; i < entities.size(); i++) {
        int childID = entities[i];
        instances[childID].parentID = parentID;
    }

    Instance newInstance;
    instances.push_back(newInstance);
    return instances.size() - 1;

}


int Scene::Tesseract(glm::vec3 color, bool inverted){
    std::vector<Vertex> vertexData = 
    {

    // Cell #0 --------------------------------------------

        { {-0.5, -0.5, -0.5, -0.5}, color, {-1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, -0.5, 0.5, 0.5}, color, {-1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, -0.5, 0.5}, color, {-1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, 0.5, -0.5}, color, {-1, 0, 0, 0}, {0.f, 0.f, 0.f} },

        { {-0.5, -0.5, -0.5, -0.5}, color, {-1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, -0.5, -0.5, 0.5}, color, {-1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, -0.5, 0.5, 0.5}, color, {-1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, -0.5, 0.5}, color, {-1, 0, 0, 0}, {0.f, 0.f, 0.f} },

        { {-0.5, -0.5, -0.5, -0.5}, color, {-1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, -0.5, 0.5, -0.5}, color, {-1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, -0.5, 0.5, 0.5}, color, {-1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, 0.5, -0.5}, color, {-1, 0, 0, 0}, {0.f, 0.f, 0.f} },

        { {-0.5, -0.5, 0.5, 0.5}, color, {-1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, -0.5, 0.5}, color, {-1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, 0.5, -0.5}, color, {-1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, 0.5, 0.5}, color, {-1, 0, 0, 0}, {0.f, 0.f, 0.f} },

        { {-0.5, -0.5, -0.5, -0.5}, color, {-1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, -0.5, -0.5}, color, {-1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, -0.5, 0.5}, color, {-1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, 0.5, -0.5}, color, {-1, 0, 0, 0}, {0.f, 0.f, 0.f} },

    // Cell #1 --------------------------------------------

        { {0.5, -0.5, -0.5, -0.5}, color, {1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, 0.5}, color, {1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, 0.5}, color, {1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, 0.5, -0.5}, color, {1, 0, 0, 0}, {0.f, 0.f, 0.f} },

        { {0.5, -0.5, -0.5, -0.5}, color, {1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, -0.5, 0.5}, color, {1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, 0.5}, color, {1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, 0.5}, color, {1, 0, 0, 0}, {0.f, 0.f, 0.f} },

        { {0.5, -0.5, -0.5, -0.5}, color, {1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, -0.5}, color, {1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, 0.5}, color, {1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, 0.5, -0.5}, color, {1, 0, 0, 0}, {0.f, 0.f, 0.f} },

        { {0.5, -0.5, 0.5, 0.5}, color, {1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, 0.5}, color, {1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, 0.5, -0.5}, color, {1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, 0.5, 0.5}, color, {1, 0, 0, 0}, {0.f, 0.f, 0.f} },

        { {0.5, -0.5, -0.5, -0.5}, color, {1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, -0.5}, color, {1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, 0.5}, color, {1, 0, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, 0.5, -0.5}, color, {1, 0, 0, 0}, {0.f, 0.f, 0.f} },

    // Cell #2 --------------------------------------------

        { {-0.5, -0.5, -0.5, -0.5}, color, {0, -1, 0, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, -0.5, 0.5, 0.5}, color, {0, -1, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, -0.5, 0.5}, color, {0, -1, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, -0.5}, color, {0, -1, 0, 0}, {0.f, 0.f, 0.f} },

        { {-0.5, -0.5, -0.5, -0.5}, color, {0, -1, 0, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, -0.5, -0.5, 0.5}, color, {0, -1, 0, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, -0.5, 0.5, 0.5}, color, {0, -1, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, -0.5, 0.5}, color, {0, -1, 0, 0}, {0.f, 0.f, 0.f} },

        { {-0.5, -0.5, -0.5, -0.5}, color, {0, -1, 0, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, -0.5, 0.5, -0.5}, color, {0, -1, 0, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, -0.5, 0.5, 0.5}, color, {0, -1, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, -0.5}, color, {0, -1, 0, 0}, {0.f, 0.f, 0.f} },

        { {-0.5, -0.5, 0.5, 0.5}, color, {0, -1, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, -0.5, 0.5}, color, {0, -1, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, -0.5}, color, {0, -1, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, 0.5}, color, {0, -1, 0, 0}, {0.f, 0.f, 0.f} },

        { {-0.5, -0.5, -0.5, -0.5}, color, {0, -1, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, -0.5, -0.5}, color, {0, -1, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, -0.5, 0.5}, color, {0, -1, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, -0.5}, color, {0, -1, 0, 0}, {0.f, 0.f, 0.f} },

    // Cell #3 --------------------------------------------

        { {-0.5, 0.5, -0.5, -0.5}, color, {0, 1, 0, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 1, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, 0.5}, color, {0, 1, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, 0.5, -0.5}, color, {0, 1, 0, 0}, {0.f, 0.f, 0.f} },

        { {-0.5, 0.5, -0.5, -0.5}, color, {0, 1, 0, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, -0.5, 0.5}, color, {0, 1, 0, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 1, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, 0.5}, color, {0, 1, 0, 0}, {0.f, 0.f, 0.f} },

        { {-0.5, 0.5, -0.5, -0.5}, color, {0, 1, 0, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, 0.5, -0.5}, color, {0, 1, 0, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 1, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, 0.5, -0.5}, color, {0, 1, 0, 0}, {0.f, 0.f, 0.f} },

        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 1, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, 0.5}, color, {0, 1, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, 0.5, -0.5}, color, {0, 1, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, 0.5, 0.5}, color, {0, 1, 0, 0}, {0.f, 0.f, 0.f} },

        { {-0.5, 0.5, -0.5, -0.5}, color, {0, 1, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, -0.5}, color, {0, 1, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, 0.5}, color, {0, 1, 0, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, 0.5, -0.5}, color, {0, 1, 0, 0}, {0.f, 0.f, 0.f} },

    // Cell #4 --------------------------------------------

        { {-0.5, -0.5, -0.5, -0.5}, color, {0, 0, -1, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, -0.5, 0.5}, color, {0, 0, -1, 0}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, -0.5, 0.5}, color, {0, 0, -1, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, -0.5}, color, {0, 0, -1, 0}, {0.f, 0.f, 0.f} },

        { {-0.5, -0.5, -0.5, -0.5}, color, {0, 0, -1, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, -0.5, -0.5, 0.5}, color, {0, 0, -1, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, -0.5, 0.5}, color, {0, 0, -1, 0}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, -0.5, 0.5}, color, {0, 0, -1, 0}, {0.f, 0.f, 0.f} },

        { {-0.5, -0.5, -0.5, -0.5}, color, {0, 0, -1, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, -0.5, -0.5}, color, {0, 0, -1, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, -0.5, 0.5}, color, {0, 0, -1, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, -0.5}, color, {0, 0, -1, 0}, {0.f, 0.f, 0.f} },

        { {-0.5, 0.5, -0.5, 0.5}, color, {0, 0, -1, 0}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, -0.5, 0.5}, color, {0, 0, -1, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, -0.5}, color, {0, 0, -1, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, 0.5}, color, {0, 0, -1, 0}, {0.f, 0.f, 0.f} },

        { {-0.5, -0.5, -0.5, -0.5}, color, {0, 0, -1, 0}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, -0.5, -0.5}, color, {0, 0, -1, 0}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, -0.5, 0.5}, color, {0, 0, -1, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, -0.5}, color, {0, 0, -1, 0}, {0.f, 0.f, 0.f} },

    // Cell #5 --------------------------------------------

        { {-0.5, -0.5, 0.5, -0.5}, color, {0, 0, 1, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 0, 1, 0}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, 0.5}, color, {0, 0, 1, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, 0.5, -0.5}, color, {0, 0, 1, 0}, {0.f, 0.f, 0.f} },

        { {-0.5, -0.5, 0.5, -0.5}, color, {0, 0, 1, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, -0.5, 0.5, 0.5}, color, {0, 0, 1, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 0, 1, 0}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, 0.5}, color, {0, 0, 1, 0}, {0.f, 0.f, 0.f} },

        { {-0.5, -0.5, 0.5, -0.5}, color, {0, 0, 1, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, 0.5, -0.5}, color, {0, 0, 1, 0}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 0, 1, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, 0.5, -0.5}, color, {0, 0, 1, 0}, {0.f, 0.f, 0.f} },

        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 0, 1, 0}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, 0.5}, color, {0, 0, 1, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, 0.5, -0.5}, color, {0, 0, 1, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, 0.5, 0.5}, color, {0, 0, 1, 0}, {0.f, 0.f, 0.f} },

        { {-0.5, -0.5, 0.5, -0.5}, color, {0, 0, 1, 0}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, -0.5}, color, {0, 0, 1, 0}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, 0.5}, color, {0, 0, 1, 0}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, 0.5, -0.5}, color, {0, 0, 1, 0}, {0.f, 0.f, 0.f} },

    // Cell #6 --------------------------------------------

        { {-0.5, -0.5, -0.5, -0.5}, color, {0, 0, 0, -1}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, 0.5, -0.5}, color, {0, 0, 0, -1}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, -0.5}, color, {0, 0, 0, -1}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, -0.5}, color, {0, 0, 0, -1}, {0.f, 0.f, 0.f} },

        { {-0.5, -0.5, -0.5, -0.5}, color, {0, 0, 0, -1}, {0.f, 0.f, 0.f} },
        { {-0.5, -0.5, 0.5, -0.5}, color, {0, 0, 0, -1}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, 0.5, -0.5}, color, {0, 0, 0, -1}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, -0.5}, color, {0, 0, 0, -1}, {0.f, 0.f, 0.f} },

        { {-0.5, -0.5, -0.5, -0.5}, color, {0, 0, 0, -1}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, -0.5, -0.5}, color, {0, 0, 0, -1}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, 0.5, -0.5}, color, {0, 0, 0, -1}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, -0.5}, color, {0, 0, 0, -1}, {0.f, 0.f, 0.f} },

        { {-0.5, 0.5, 0.5, -0.5}, color, {0, 0, 0, -1}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, -0.5}, color, {0, 0, 0, -1}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, -0.5}, color, {0, 0, 0, -1}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, 0.5, -0.5}, color, {0, 0, 0, -1}, {0.f, 0.f, 0.f} },

        { {-0.5, -0.5, -0.5, -0.5}, color, {0, 0, 0, -1}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, -0.5, -0.5}, color, {0, 0, 0, -1}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, -0.5}, color, {0, 0, 0, -1}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, -0.5}, color, {0, 0, 0, -1}, {0.f, 0.f, 0.f} },

    // Cell #7 --------------------------------------------

        { {-0.5, -0.5, -0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },

        { {-0.5, -0.5, -0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {-0.5, -0.5, 0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },

        { {-0.5, -0.5, -0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, -0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },

        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, 0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },

        { {-0.5, -0.5, -0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, -0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} }

    };

    if (inverted) {
        for (int i = 0; i < vertexData.size(); i++) {
            vertexData[i].normal *= -1.f;
        }
    }

    std::vector<glm::vec3> minUVCorners =
    {
        {0.f, 0.5f, 0.3333f},
        {0.6666f, 0.5f, 0.3333f},
        {0.3333f, 0.3333f, 0.3333f},
        {0.3333f, 0.75f, 0.3333f},
        {0.3333f, 0.5f, 0.6666f},
        {0.3333f, 0.5f, 0.f},
        {0.3333f, 0.f, 0.3333f},
        {0.3333f, 0.5f, 0.3333f}
    };
    
    glm::vec3 uvDiagonal(0.3333f, 0.25f, 0.3333f);

    for (int i = 0; i < 8; i++) {
        
        for (int j = 0; j < 20; j++) {
            glm::vec3 minCorner = minUVCorners[i];
            glm::vec3 maxCorner = minCorner + uvDiagonal;
            Vertex v = vertexData[20 * i + j];
            int curr = 0;

            for (int k = 0; k < 4; k++) {
                if (k != i / 2) {
                    if (v.pos[k] > 0.f) {
                        v.texCoord[curr] = maxCorner[curr];
                    }
                    else {
                        v.texCoord[curr] = minCorner[curr];
                    }
                    curr++;
                }
            }
            vertexData[20 * i + j] = v;
        }
    }


    return createEntity(vertexData, inverted);
}


int Scene::Cube(glm::vec3 color) {
    std::vector<Vertex> vertexData = {
        { {-0.5, -0.5, -0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },

        { {-0.5, -0.5, -0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {-0.5, -0.5, 0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },

        { {-0.5, -0.5, -0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, -0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },

        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, 0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },

        { {-0.5, -0.5, -0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, -0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {0.5, -0.5, 0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} },
        { {0.5, 0.5, -0.5, 0.5}, color, {0, 0, 0, 1}, {0.f, 0.f, 0.f} }
    };

    for (int i = 0; i < vertexData.size(); i++) {
        vertexData[i].pos[3] = 0.f;
        int currIndex = 0;
        for (int j = 0; j < 3; j++) {
            vertexData[i].texCoord[j] = vertexData[i].pos[j] < 0.f ? 0.f : 1.f;
        }
    }

    return createEntity(vertexData, false, glm::vec4(0.f, 0.f, 0.f, -0.5f));
}
