#include "Scene.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>
#include "Vertex.h"

Scene::Scene(){}

void Scene::initialize(VkCommandPool& cmdPool, VkQueue& queue, VkDevice& logicalDevice, VkPhysicalDevice& physicalDevice){
    this->cmdPool = cmdPool;
    this->queue = queue;
    this->logicalDevice = logicalDevice;
    this->physicalDevice = physicalDevice;
}


int Scene::createEntity(std::vector<Vertex> vData, std::vector<uint16_t> iData){
    Entity newEntity(vData, iData, cmdPool, queue, logicalDevice, physicalDevice);
    entities.push_back(newEntity);
    return entities.size() - 1;
}

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

int Scene::Tesseract(glm::vec3 color){
    /*
    std::vector<HyperVertex> vertexData;

    for(int i = 0; i < 8; i++){
        glm::vec4 currNormal = glm::vec4(0.f);
        currNormal[i / 2] = (float)(i % 2 * 2 - 1);
        glm::vec4 currPos = glm::vec4(0.f);
        currPos[i / 2] = (float)(i % 2 * 2 - 1) * 0.5f;

        for(int j = 0; j < 8; j++){
            glm::vec4 currPosCopy = currPos;

            glm::vec3 pos(-1.f);
            int sum = j;
            for(int k = 0; k < 3; k++){
                if(sum >=  pow(2, 2 - k)){
                    pos[k] *= -1.f;
                    sum -= pow(2, 2 - k);
                }
            }
            pos *= 0.5f;

            int index = 0;
            for(int k = 0; k < 4; k++){
                if(k != i / 2){
                    currPosCopy[k] = pos[index];
                    index++;
                }
            }

            vertexData.push_back({currPosCopy, color, currNormal});
        }
    }

    std::vector<HyperVertex> secondPass;
    std::vector<unsigned> perCellIndices = {0, 3, 5, 6, 0, 1, 3, 5, 0, 2, 3, 6, 3, 5, 6, 7, 0, 4, 5, 6};

    for(int i = 0; i < 8; i++){
        //i * 8 to (i + 1) * 8 - 1
        for(int j = 0; j < perCellIndices.size(); j++){
            secondPass.push_back(vertexData[perCellIndices[j] + i * 8]);
        }
    }

    vertexData = secondPass;

    std::cout << "{" << std::endl;
    for(int i = 0; i < vertexData.size(); i++){
        if(i % perCellIndices.size() == 0){
            std::cout << "// Cell #" << i / perCellIndices.size() << " --------------------------------------------" << std::endl << std::endl;
        }
        std::cout << "     { {";
        std::cout << vertexData[i].pos.x << ", " << vertexData[i].pos.y << ", " << vertexData[i].pos.z << ", " << vertexData[i].pos.w;
        std::cout << "}, color, {";
        std::cout << vertexData[i].normal.x << ", " << vertexData[i].normal.y << ", " << vertexData[i].normal.z << ", " << vertexData[i].normal.w;
        std::cout << "} }";

        if(i != vertexData.size() - 1){
            std::cout << ",";
        }

        if(i % perCellIndices.size() == perCellIndices.size() - 1 || i % 4 == 3){
            std::cout << std::endl;
        }

        std::cout << std::endl;
    }
    std::cout << "};";
    */

    std::vector<HyperVertex> vertexData = 
    {

    // Cell #0 --------------------------------------------

        { {-0.5, -0.5, -0.5, -0.5}, color, {-1, 0, 0, 0} },
        { {-0.5, -0.5, 0.5, 0.5}, color, {-1, 0, 0, 0} },
        { {-0.5, 0.5, -0.5, 0.5}, color, {-1, 0, 0, 0} },
        { {-0.5, 0.5, 0.5, -0.5}, color, {-1, 0, 0, 0} },

        { {-0.5, -0.5, -0.5, -0.5}, color, {-1, 0, 0, 0} },
        { {-0.5, -0.5, -0.5, 0.5}, color, {-1, 0, 0, 0} },
        { {-0.5, -0.5, 0.5, 0.5}, color, {-1, 0, 0, 0} },
        { {-0.5, 0.5, -0.5, 0.5}, color, {-1, 0, 0, 0} },

        { {-0.5, -0.5, -0.5, -0.5}, color, {-1, 0, 0, 0} },
        { {-0.5, -0.5, 0.5, -0.5}, color, {-1, 0, 0, 0} },
        { {-0.5, -0.5, 0.5, 0.5}, color, {-1, 0, 0, 0} },
        { {-0.5, 0.5, 0.5, -0.5}, color, {-1, 0, 0, 0} },

        { {-0.5, -0.5, 0.5, 0.5}, color, {-1, 0, 0, 0} },
        { {-0.5, 0.5, -0.5, 0.5}, color, {-1, 0, 0, 0} },
        { {-0.5, 0.5, 0.5, -0.5}, color, {-1, 0, 0, 0} },
        { {-0.5, 0.5, 0.5, 0.5}, color, {-1, 0, 0, 0} },

        { {-0.5, -0.5, -0.5, -0.5}, color, {-1, 0, 0, 0} },
        { {-0.5, 0.5, -0.5, -0.5}, color, {-1, 0, 0, 0} },
        { {-0.5, 0.5, -0.5, 0.5}, color, {-1, 0, 0, 0} },
        { {-0.5, 0.5, 0.5, -0.5}, color, {-1, 0, 0, 0} },

    // Cell #1 --------------------------------------------

        { {0.5, -0.5, -0.5, -0.5}, color, {1, 0, 0, 0} },
        { {0.5, -0.5, 0.5, 0.5}, color, {1, 0, 0, 0} },
        { {0.5, 0.5, -0.5, 0.5}, color, {1, 0, 0, 0} },
        { {0.5, 0.5, 0.5, -0.5}, color, {1, 0, 0, 0} },

        { {0.5, -0.5, -0.5, -0.5}, color, {1, 0, 0, 0} },
        { {0.5, -0.5, -0.5, 0.5}, color, {1, 0, 0, 0} },
        { {0.5, -0.5, 0.5, 0.5}, color, {1, 0, 0, 0} },
        { {0.5, 0.5, -0.5, 0.5}, color, {1, 0, 0, 0} },

        { {0.5, -0.5, -0.5, -0.5}, color, {1, 0, 0, 0} },
        { {0.5, -0.5, 0.5, -0.5}, color, {1, 0, 0, 0} },
        { {0.5, -0.5, 0.5, 0.5}, color, {1, 0, 0, 0} },
        { {0.5, 0.5, 0.5, -0.5}, color, {1, 0, 0, 0} },

        { {0.5, -0.5, 0.5, 0.5}, color, {1, 0, 0, 0} },
        { {0.5, 0.5, -0.5, 0.5}, color, {1, 0, 0, 0} },
        { {0.5, 0.5, 0.5, -0.5}, color, {1, 0, 0, 0} },
        { {0.5, 0.5, 0.5, 0.5}, color, {1, 0, 0, 0} },

        { {0.5, -0.5, -0.5, -0.5}, color, {1, 0, 0, 0} },
        { {0.5, 0.5, -0.5, -0.5}, color, {1, 0, 0, 0} },
        { {0.5, 0.5, -0.5, 0.5}, color, {1, 0, 0, 0} },
        { {0.5, 0.5, 0.5, -0.5}, color, {1, 0, 0, 0} },

    // Cell #2 --------------------------------------------

        { {-0.5, -0.5, -0.5, -0.5}, color, {0, -1, 0, 0} },
        { {-0.5, -0.5, 0.5, 0.5}, color, {0, -1, 0, 0} },
        { {0.5, -0.5, -0.5, 0.5}, color, {0, -1, 0, 0} },
        { {0.5, -0.5, 0.5, -0.5}, color, {0, -1, 0, 0} },

        { {-0.5, -0.5, -0.5, -0.5}, color, {0, -1, 0, 0} },
        { {-0.5, -0.5, -0.5, 0.5}, color, {0, -1, 0, 0} },
        { {-0.5, -0.5, 0.5, 0.5}, color, {0, -1, 0, 0} },
        { {0.5, -0.5, -0.5, 0.5}, color, {0, -1, 0, 0} },

        { {-0.5, -0.5, -0.5, -0.5}, color, {0, -1, 0, 0} },
        { {-0.5, -0.5, 0.5, -0.5}, color, {0, -1, 0, 0} },
        { {-0.5, -0.5, 0.5, 0.5}, color, {0, -1, 0, 0} },
        { {0.5, -0.5, 0.5, -0.5}, color, {0, -1, 0, 0} },

        { {-0.5, -0.5, 0.5, 0.5}, color, {0, -1, 0, 0} },
        { {0.5, -0.5, -0.5, 0.5}, color, {0, -1, 0, 0} },
        { {0.5, -0.5, 0.5, -0.5}, color, {0, -1, 0, 0} },
        { {0.5, -0.5, 0.5, 0.5}, color, {0, -1, 0, 0} },

        { {-0.5, -0.5, -0.5, -0.5}, color, {0, -1, 0, 0} },
        { {0.5, -0.5, -0.5, -0.5}, color, {0, -1, 0, 0} },
        { {0.5, -0.5, -0.5, 0.5}, color, {0, -1, 0, 0} },
        { {0.5, -0.5, 0.5, -0.5}, color, {0, -1, 0, 0} },

    // Cell #3 --------------------------------------------

        { {-0.5, 0.5, -0.5, -0.5}, color, {0, 1, 0, 0} },
        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 1, 0, 0} },
        { {0.5, 0.5, -0.5, 0.5}, color, {0, 1, 0, 0} },
        { {0.5, 0.5, 0.5, -0.5}, color, {0, 1, 0, 0} },

        { {-0.5, 0.5, -0.5, -0.5}, color, {0, 1, 0, 0} },
        { {-0.5, 0.5, -0.5, 0.5}, color, {0, 1, 0, 0} },
        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 1, 0, 0} },
        { {0.5, 0.5, -0.5, 0.5}, color, {0, 1, 0, 0} },

        { {-0.5, 0.5, -0.5, -0.5}, color, {0, 1, 0, 0} },
        { {-0.5, 0.5, 0.5, -0.5}, color, {0, 1, 0, 0} },
        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 1, 0, 0} },
        { {0.5, 0.5, 0.5, -0.5}, color, {0, 1, 0, 0} },

        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 1, 0, 0} },
        { {0.5, 0.5, -0.5, 0.5}, color, {0, 1, 0, 0} },
        { {0.5, 0.5, 0.5, -0.5}, color, {0, 1, 0, 0} },
        { {0.5, 0.5, 0.5, 0.5}, color, {0, 1, 0, 0} },

        { {-0.5, 0.5, -0.5, -0.5}, color, {0, 1, 0, 0} },
        { {0.5, 0.5, -0.5, -0.5}, color, {0, 1, 0, 0} },
        { {0.5, 0.5, -0.5, 0.5}, color, {0, 1, 0, 0} },
        { {0.5, 0.5, 0.5, -0.5}, color, {0, 1, 0, 0} },

    // Cell #4 --------------------------------------------

        { {-0.5, -0.5, -0.5, -0.5}, color, {0, 0, -1, 0} },
        { {-0.5, 0.5, -0.5, 0.5}, color, {0, 0, -1, 0} },
        { {0.5, -0.5, -0.5, 0.5}, color, {0, 0, -1, 0} },
        { {0.5, 0.5, -0.5, -0.5}, color, {0, 0, -1, 0} },

        { {-0.5, -0.5, -0.5, -0.5}, color, {0, 0, -1, 0} },
        { {-0.5, -0.5, -0.5, 0.5}, color, {0, 0, -1, 0} },
        { {-0.5, 0.5, -0.5, 0.5}, color, {0, 0, -1, 0} },
        { {0.5, -0.5, -0.5, 0.5}, color, {0, 0, -1, 0} },

        { {-0.5, -0.5, -0.5, -0.5}, color, {0, 0, -1, 0} },
        { {-0.5, 0.5, -0.5, -0.5}, color, {0, 0, -1, 0} },
        { {-0.5, 0.5, -0.5, 0.5}, color, {0, 0, -1, 0} },
        { {0.5, 0.5, -0.5, -0.5}, color, {0, 0, -1, 0} },

        { {-0.5, 0.5, -0.5, 0.5}, color, {0, 0, -1, 0} },
        { {0.5, -0.5, -0.5, 0.5}, color, {0, 0, -1, 0} },
        { {0.5, 0.5, -0.5, -0.5}, color, {0, 0, -1, 0} },
        { {0.5, 0.5, -0.5, 0.5}, color, {0, 0, -1, 0} },

        { {-0.5, -0.5, -0.5, -0.5}, color, {0, 0, -1, 0} },
        { {0.5, -0.5, -0.5, -0.5}, color, {0, 0, -1, 0} },
        { {0.5, -0.5, -0.5, 0.5}, color, {0, 0, -1, 0} },
        { {0.5, 0.5, -0.5, -0.5}, color, {0, 0, -1, 0} },

    // Cell #5 --------------------------------------------

        { {-0.5, -0.5, 0.5, -0.5}, color, {0, 0, 1, 0} },
        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 0, 1, 0} },
        { {0.5, -0.5, 0.5, 0.5}, color, {0, 0, 1, 0} },
        { {0.5, 0.5, 0.5, -0.5}, color, {0, 0, 1, 0} },

        { {-0.5, -0.5, 0.5, -0.5}, color, {0, 0, 1, 0} },
        { {-0.5, -0.5, 0.5, 0.5}, color, {0, 0, 1, 0} },
        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 0, 1, 0} },
        { {0.5, -0.5, 0.5, 0.5}, color, {0, 0, 1, 0} },

        { {-0.5, -0.5, 0.5, -0.5}, color, {0, 0, 1, 0} },
        { {-0.5, 0.5, 0.5, -0.5}, color, {0, 0, 1, 0} },
        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 0, 1, 0} },
        { {0.5, 0.5, 0.5, -0.5}, color, {0, 0, 1, 0} },

        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 0, 1, 0} },
        { {0.5, -0.5, 0.5, 0.5}, color, {0, 0, 1, 0} },
        { {0.5, 0.5, 0.5, -0.5}, color, {0, 0, 1, 0} },
        { {0.5, 0.5, 0.5, 0.5}, color, {0, 0, 1, 0} },

        { {-0.5, -0.5, 0.5, -0.5}, color, {0, 0, 1, 0} },
        { {0.5, -0.5, 0.5, -0.5}, color, {0, 0, 1, 0} },
        { {0.5, -0.5, 0.5, 0.5}, color, {0, 0, 1, 0} },
        { {0.5, 0.5, 0.5, -0.5}, color, {0, 0, 1, 0} },

    // Cell #6 --------------------------------------------

        { {-0.5, -0.5, -0.5, -0.5}, color, {0, 0, 0, -1} },
        { {-0.5, 0.5, 0.5, -0.5}, color, {0, 0, 0, -1} },
        { {0.5, -0.5, 0.5, -0.5}, color, {0, 0, 0, -1} },
        { {0.5, 0.5, -0.5, -0.5}, color, {0, 0, 0, -1} },

        { {-0.5, -0.5, -0.5, -0.5}, color, {0, 0, 0, -1} },
        { {-0.5, -0.5, 0.5, -0.5}, color, {0, 0, 0, -1} },
        { {-0.5, 0.5, 0.5, -0.5}, color, {0, 0, 0, -1} },
        { {0.5, -0.5, 0.5, -0.5}, color, {0, 0, 0, -1} },

        { {-0.5, -0.5, -0.5, -0.5}, color, {0, 0, 0, -1} },
        { {-0.5, 0.5, -0.5, -0.5}, color, {0, 0, 0, -1} },
        { {-0.5, 0.5, 0.5, -0.5}, color, {0, 0, 0, -1} },
        { {0.5, 0.5, -0.5, -0.5}, color, {0, 0, 0, -1} },

        { {-0.5, 0.5, 0.5, -0.5}, color, {0, 0, 0, -1} },
        { {0.5, -0.5, 0.5, -0.5}, color, {0, 0, 0, -1} },
        { {0.5, 0.5, -0.5, -0.5}, color, {0, 0, 0, -1} },
        { {0.5, 0.5, 0.5, -0.5}, color, {0, 0, 0, -1} },

        { {-0.5, -0.5, -0.5, -0.5}, color, {0, 0, 0, -1} },
        { {0.5, -0.5, -0.5, -0.5}, color, {0, 0, 0, -1} },
        { {0.5, -0.5, 0.5, -0.5}, color, {0, 0, 0, -1} },
        { {0.5, 0.5, -0.5, -0.5}, color, {0, 0, 0, -1} },

    // Cell #7 --------------------------------------------

        { {-0.5, -0.5, -0.5, 0.5}, color, {0, 0, 0, 1} },
        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 0, 0, 1} },
        { {0.5, -0.5, 0.5, 0.5}, color, {0, 0, 0, 1} },
        { {0.5, 0.5, -0.5, 0.5}, color, {0, 0, 0, 1} },

        { {-0.5, -0.5, -0.5, 0.5}, color, {0, 0, 0, 1} },
        { {-0.5, -0.5, 0.5, 0.5}, color, {0, 0, 0, 1} },
        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 0, 0, 1} },
        { {0.5, -0.5, 0.5, 0.5}, color, {0, 0, 0, 1} },

        { {-0.5, -0.5, -0.5, 0.5}, color, {0, 0, 0, 1} },
        { {-0.5, 0.5, -0.5, 0.5}, color, {0, 0, 0, 1} },
        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 0, 0, 1} },
        { {0.5, 0.5, -0.5, 0.5}, color, {0, 0, 0, 1} },

        { {-0.5, 0.5, 0.5, 0.5}, color, {0, 0, 0, 1} },
        { {0.5, -0.5, 0.5, 0.5}, color, {0, 0, 0, 1} },
        { {0.5, 0.5, -0.5, 0.5}, color, {0, 0, 0, 1} },
        { {0.5, 0.5, 0.5, 0.5}, color, {0, 0, 0, 1} },

        { {-0.5, -0.5, -0.5, 0.5}, color, {0, 0, 0, 1} },
        { {0.5, -0.5, -0.5, 0.5}, color, {0, 0, 0, 1} },
        { {0.5, -0.5, 0.5, 0.5}, color, {0, 0, 0, 1} },
        { {0.5, 0.5, -0.5, 0.5}, color, {0, 0, 0, 1} }

};

}



/*    std::cout << "{" << std::endl;
    for(int i = 0; i < vertexData.size(); i++){
        std::cout << "     { {";
        std::cout << vertexData[i].pos.x << ", " << vertexData[i].pos.y << ", " << vertexData[i].pos.z << ", " << vertexData[i].pos.w;
        std::cout << "}, color, {";
        std::cout << vertexData[i].normal.x << ", " << vertexData[i].normal.y << ", " << vertexData[i].normal.z << ", " << vertexData[i].normal.w;
        std::cout << "} }";
        if(i != vertexData.size() - 1){
            std::cout << ",";
        }
        std::cout << "     //" << i << std::endl;
    }
    std::cout << "};";*/