#include "Scene.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>
#include <iostream>

Scene::Scene(){

    std::cout << "Vertex size: " << sizeof(Vertex) << std::endl;
    std::cout << "pos offset: " << offsetof(Vertex, pos) << std::endl;
    std::cout << "color offset: " << offsetof(Vertex, color) << std::endl;
    std::cout << "normal offset: " << offsetof(Vertex, normal) << std::endl;
    std::cout << "texCoord offset: " << offsetof(Vertex, texCoord) << std::endl;
    std::cout << "instanceID offset: " << offsetof(Vertex, instanceID) << std::endl;
}


int Scene::createEntity(std::vector<Vertex> vData){
    correctWindingOrder(vData);
    for (int i = 0; i < vData.size(); i++) {
        Vertex newVertex = vData[i];
        newVertex.instanceID.x = instances.size();
        vertices.push_back(newVertex);
    }

    
    Instance newInstance;
    instances.push_back(newInstance);

    return instances.size() - 1;
}

std::vector<Scene::ConvertedInstance> Scene::convertInstances() {
    std::vector<ConvertedInstance> converted;
    for (int i = 0; i < instances.size(); i++) {
        glm::vec4 scaleVec = instances[i].currScale;
        glm::mat4 scale(1.f);
        for (int j = 0; j < 4; j++) {
            scale[j][j] = scaleVec[j];
        }
        converted.push_back({ instances[i].currRotation.toMatrix() * scale, instances[i].currTranslate });
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

void Scene::correctWindingOrder(std::vector<Vertex>& vertexData, glm::vec4 center) {
    for (int i = 0; i < vertexData.size(); i += 4) {
        glm::vec4 vA = vertexData[i].pos - vertexData[i + 1].pos;
        glm::vec4 vB = vertexData[i].pos -vertexData[i + 2].pos;
        glm::vec4 vC = vertexData[i].pos - vertexData[i + 3].pos;
        glm::vec4 normal = crossProduct4D(vA, vB, vC);

        glm::vec4 cellCenter = (vertexData[i].pos + vertexData[i + 1].pos + vertexData[i + 2].pos + vertexData[i + 3].pos) / 4.f;
        glm::vec4 outwards = cellCenter - center;

        if (glm::dot(outwards, normal) < 0) {
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

void Scene::rotate(int entityIndex, int x, int y, int z, int w, float radians) {
    Rotor rotation;
    float rotVal = glm::sin(radians / 2.f);
    
    if (x && y) {
        rotation.setXY(rotVal);
    } else if (x && z) {
        rotation.setXZ(rotVal);
    } else if (x && w) {
        rotation.setXW(rotVal);
    } else if (y && z) {
        rotation.setYZ(rotVal);
    } else if (y && w) {
        rotation.setYW(rotVal);
    } else if (z && w) {
        rotation.setZW(rotVal);
    } else {
        std::cout << "Invalid rotation" << std::endl;
        return;
    }

    instances[entityIndex].currRotation = instances[entityIndex].currRotation * rotation;

}

/*
int Scene::Cube(glm::vec3 color){
    std::vector<Vertex> vertexData = 
    {

        //    2 --------- 6 
        //   /|          /|           
        //  3 --------- 7 |
        //  | |         | |
        //  | |         | |
        //  | 0 --------| 4
        //  |/          |/
        //  1 --------- 5

        //0
        {{-0.5f, -0.5f, -0.5f}, color, {-1.f, 0.f, 0.f}},
        {{-0.5f, -0.5f, -0.5f}, color, {0.f, -1.f, 0.f}},
        {{-0.5f, -0.5f, -0.5f}, color, {0.f, 0.f, -1.f}},

        //1
        {{-0.5f, -0.5f, 0.5f}, color, {-1.f, 0.f, 0.f}},
        {{-0.5f, -0.5f, 0.5f}, color, {0.f, -1.f, 0.f}},
        {{-0.5f, -0.5f, 0.5f}, color, {0.f, 0.f, 1.f}},

        //2
        {{-0.5f, 0.5f, -0.5f}, color, {-1.f, 0.f, 0.f}},
        {{-0.5f, 0.5f, -0.5f}, color, {0.f, 1.f, 0.f}},
        {{-0.5f, 0.5f, -0.5f}, color, {0.f, 0.f, -1.f}},

        //3
        {{-0.5f, 0.5f, 0.5f}, color, {-1.f, 0.f, 0.f}}, 
        {{-0.5f, 0.5f, 0.5f}, color, {0.f, 1.f, 0.f}},
        {{-0.5f, 0.5f, 0.5f}, color, {0.f, 0.f, 1.f}},

        //4
        {{0.5f, -0.5f, -0.5f}, color, {1.f, 0.f, 0.f}},
        {{0.5f, -0.5f, -0.5f}, color, {0.f, -1.f, 0.f}},
        {{0.5f, -0.5f, -0.5f}, color, {0.f, 0.f, -1.f}},

        //5
        {{0.5f, -0.5f, 0.5f}, color, {1.f, 0.f, 0.f}}, 
        {{0.5f, -0.5f, 0.5f}, color, {0.f, -1.f, 0.f}},
        {{0.5f, -0.5f, 0.5f}, color, {0.f, 0.f, 1.f}},

        //6
        {{0.5f, 0.5f, -0.5f}, color, {1.f, 0.f, 0.f}}, 
        {{0.5f, 0.5f, -0.5f}, color, {0.f, 1.f, 0.f}},
        {{0.5f, 0.5f, -0.5f}, color, {0.f, 0.f, -1.f}},

        //7
        {{0.5f, 0.5f, 0.5f}, color, {1.f, 0.f, 0.f}},
        {{0.5f, 0.5f, 0.5f}, color, {0.f, 1.f, 0.f}}, 
        {{0.5f, 0.5f, 0.5f}, color, {0.f, 0.f, 1.f}}      
    };

    std::vector<uint16_t> indexData{
        //  3-----7   7-----6   6-----2   
        //  |  F  |   |  R  |   |  B  |
        //  1-----5   5-----4   4-----0
        //  2-----3   2-----6   1-----5
        //  |  L  |   |  U  |   |  D  |
        //  0-----1   3-----7   0-----4

        0, 6, 9, 0, 9, 3,       //L
        5, 11, 23, 5, 23, 17,   //F
        15, 21, 18, 15, 18, 12, //R
        14, 20, 8, 14, 8, 2,    //B
        10, 7, 19, 10, 19, 22,  //U
        1, 4, 16, 1, 16, 13     //B
    };

    return createEntity(vertexData, indexData);
}

int Scene::Cylinder(glm::vec3 color){
    float r = 1.0f;
    float h = 1.0f;

    float currX = r;
    float currZ = 0.0f;
    float currY = h/2.f;
    float tempX;

    float topR = 0.0f;
    float topG = 1.0f;
    float topB = 1.0f;
    float downR = 0.0f;
    float downG = 0.0f;
    float downB = 1.0f;

    int numTriangles = 50;
    std::vector<Vertex> vertexData;
    float theta = 2 * glm::pi<float>() / numTriangles;

    //top center vertex
    vertexData.push_back({{0.f, currY, 0.f}, {topR, topG, topB}});

    //bottom center vertex
    vertexData.push_back({{0.f, -currY, 0.f}, {downR, downG, downB}});

    for(int i = 0;i < numTriangles; i++){
        vertexData.push_back({{currX, currY, currZ}, {topR, topG, topB}});
        vertexData.push_back({{currX, -currY, currZ}, {downR, downG, downB}});

        //update
        tempX = glm::cos(theta) * currX + glm::sin(theta) * currZ;
        currZ = -1 * (glm::sin(theta) * currX) + glm::cos(theta) * currZ;
        currX = tempX;
    }

    std::vector<uint16_t> indexData;
    for(int i = 0; i < numTriangles - 1; i++){
        indexData.push_back(0);             //top triangle
        indexData.push_back(2 * i + 2);
        indexData.push_back(2 * i + 4);

        indexData.push_back(1);             //bottom triangle
        indexData.push_back(2 * i + 5);
        indexData.push_back(2 * i + 3);

        indexData.push_back(2 * i + 2);     //height triangle #1
        indexData.push_back(2 * i + 3);
        indexData.push_back(2 * i + 5);

        indexData.push_back(2 * i + 2);     //height triangle #2
        indexData.push_back(2 * i + 5);
        indexData.push_back(2 * i + 4);
    }

    indexData.push_back(0);
    indexData.push_back(2 * numTriangles);
    indexData.push_back(2);

    indexData.push_back(1);
    indexData.push_back(3);
    indexData.push_back(2 * numTriangles + 1);

    indexData.push_back(2 * numTriangles);
    indexData.push_back(2 * numTriangles + 1);
    indexData.push_back(3);

    indexData.push_back(2 * numTriangles);
    indexData.push_back(3);
    indexData.push_back(2);

   return createEntity(vertexData, indexData);
}

int Scene::Sphere(glm::vec3 color){
    int numTriangles = 10;
    std::vector<Vertex> vertexData;
    std::vector<uint16_t> indexData;
    float theta = glm::pi<float>() / (float)numTriangles;

    float currX = 0.0f;
    float currY = 1.0f;
    float currZ = 0.0f;
    float tempX;

    int currLeft;
    int currRight;

    for(int i = 0; i < 2 * numTriangles - 1; i++){
        for(int j = 0; j < numTriangles - 1; j++){
            currX = 0.0f;
            currY = 1.0f;
            currZ = 0.0f;  
            //update pos
            tempX = glm::cos(-theta * (float)j) * currX - glm::sin(-theta * (float)j) * currY;
            currY = glm::sin(-theta * (float)j) * currX + glm::cos(-theta * (float)j) * currY;
            currX = tempX;

            currZ = 0.0f;
            tempX = glm::cos((float)i * theta) * currX + glm::sin((float)i * theta) * currZ;
            currZ = -1 * glm::sin((float)i * theta) * currX + glm::cos((float)i * theta) * currZ;
            currX = tempX;

            Vertex newVertex({{currX, currY, currZ}, {1.f, 0.f, 0.f}});

            vertexData.push_back(newVertex);
        }

        if(i >= 1){
            currLeft = 2 + (i - 1) * (numTriangles - 1);
            currRight = 2 + i * (numTriangles - 1);
            indexData.push_back(0);
            indexData.push_back(currLeft);
            indexData.push_back(currRight);
            for(int j = 0; j < numTriangles - 2; j++){
                indexData.push_back(currLeft);
                indexData.push_back(currLeft + 1);
                indexData.push_back(currRight);
                indexData.push_back(currLeft + 1);
                indexData.push_back(currRight + 1);
                indexData.push_back(currRight);
                currLeft++;
                currRight++;
            }
            indexData.push_back(1);
            indexData.push_back(currRight);
            indexData.push_back(currLeft);
        }
    }

    for(int j = 0; j < numTriangles - 1; j++){
        currX = 0.0f;
        currY = 1.0f;
        currZ = 0.0f;  
        //update pos
        tempX = glm::cos(-theta * (float)j) * currX - glm::sin(-theta * (float)j) * currY;
        currY = glm::sin(-theta * (float)j) * currX + glm::cos(-theta * (float)j) * currY;
        currX = tempX;

        Vertex newVertex{{currX, currY, currZ}, {1.f, 0.f, 0.f}};


        vertexData.push_back(newVertex);
    }

    currLeft = 2 + (2 * numTriangles - 2) * (numTriangles - 1);
    currRight = 2 +  (2 * numTriangles - 1) * (numTriangles - 1);
    indexData.push_back(0);
    indexData.push_back(currLeft);
    indexData.push_back(currRight);
    for(int j = 0; j < numTriangles - 2; j++){
        indexData.push_back(currLeft);
        indexData.push_back(currLeft + 1);
        indexData.push_back(currRight);
        indexData.push_back(currLeft + 1);
        indexData.push_back(currRight + 1);
        indexData.push_back(currRight);
        currLeft++;
        currRight++;
    }
    indexData.push_back(1);
    indexData.push_back(currRight);
    indexData.push_back(currLeft);


    return createEntity(vertexData, indexData);
}
*/
int Scene::Tesseract(glm::vec3 color){
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


    return createEntity(vertexData);

}
