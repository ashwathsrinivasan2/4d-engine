#define GLM_FORCE_RADIANS
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "Scene.h"
#include "Application.h"
#include "Rotor.h"

template<typename T>
inline bool contains(std::vector<T> list, T value) {
    for (int i = 0; i < list.size(); i++) {
        if (list[i] == value) {
            return true;
        }
    }
    return false;
}

inline void printVec(std::string name, glm::vec4 vec) {
    std::cout << name << ": {";
    for (int i = 0; i < 4; i++) {
        std::cout << vec[i];
        if (i != 4) std::cout << " ";
    }
    std::cout << "}" << std::endl;
}

int wallWithDoor(Scene& scene, glm::vec4 dim, glm::vec4 doorDimensions, glm::vec2 doorPosition) {

    int minIndex = 0;
    for (int i = 1; i < 4; i++) {
        if (dim[i] < dim[minIndex]) {
            minIndex = i;
        }
    }

    int aIndex = 1;
    int bIndex = 1;

    for (int i = 0; i < 4; i++) {
        if (i != 1 && i != minIndex) {
            if (aIndex == 1) {
                aIndex = i;
            }
            else {
                bIndex = i;
            }
        }
    }
    std::cout << minIndex << ", " << aIndex << ", " << bIndex << std::endl;

    doorDimensions[minIndex] = dim[minIndex];
    glm::vec4 doorPos(0.f);
    doorPos[aIndex] = doorPosition.x;
    doorPos[bIndex] = doorPosition.y;
    doorPos[1] = doorDimensions[1] / 2.f - dim[1] / 2.f;

    int aNeg = scene.Tesseract(glm::vec4(1.f), false);
    int aPos = scene.Tesseract(glm::vec4(1.f), false);
    int yPos = scene.Tesseract(glm::vec4(1.f), false);
    int bNeg = scene.Tesseract(glm::vec4(1.f), false);
    int bPos = scene.Tesseract(glm::vec4(1.f), false);



    glm::vec4 aNegScalar = dim;
    aNegScalar[aIndex] = doorPos[aIndex] + dim[aIndex] / 2.f - doorDimensions[aIndex] / 2.f;

    glm::vec4 aPosScalar = dim;
    aPosScalar[aIndex] = dim[aIndex] - (doorPos[aIndex] + dim[aIndex] / 2.f + doorDimensions[aIndex] / 2.f);

    glm::vec4 yScalar = doorDimensions;
    yScalar[1] = dim.y - doorDimensions.y;

    glm::vec4 bNegScalar = doorDimensions;
    bNegScalar[bIndex] = doorPos[bIndex] + dim[bIndex] / 2.f - doorDimensions[bIndex] / 2.f;
    bNegScalar[1] = dim.y;

    glm::vec4 bPosScalar = doorDimensions;
    bPosScalar[bIndex] = dim[bIndex] - (doorPos[bIndex] + dim[bIndex] / 2.f + doorDimensions[bIndex] / 2.f);
    bPosScalar[1] = dim.y;



    glm::vec4 aNegTranslate(0.f);
    aNegTranslate[aIndex] = doorPos[aIndex] - (doorDimensions[aIndex] / 2.f + aNegScalar[aIndex] / 2.f);

    glm::vec4 aPosTranslate(0.f);
    aPosTranslate[aIndex] = doorPos[aIndex] + doorDimensions[aIndex] / 2.f + aPosScalar[aIndex] / 2.f;

    glm::vec4 yTranslate = doorPos;
    yTranslate[1] += doorDimensions[1] / 2.f + yScalar[1] / 2.f;

    glm::vec4 bNegTranslate = doorPos;
    bNegTranslate[bIndex] -= doorDimensions[bIndex] / 2.f + bNegScalar[bIndex] / 2.f;
    bNegTranslate[1] = 0.f;

    glm::vec4 bPosTranslate = doorPos;
    bPosTranslate[bIndex] += doorDimensions[bIndex] / 2.f + bPosScalar[bIndex] / 2.f;
    bPosTranslate[1] = 0.f;

    std::vector<int> sections = { aNeg, aPos, bNeg, bPos, yPos };
    std::vector<std::string> names = { "aNeg", "aPos", "bNeg", "bPos", "y" };
    std::vector<glm::vec4> translates = { aNegTranslate, aPosTranslate, bNegTranslate, bPosTranslate, yTranslate };
    std::vector<glm::vec4> scales = { aNegScalar, aPosScalar, bNegScalar, bPosScalar, yScalar };

    for (int i = 0; i < 5; i++) {
        scene.translate(sections[i], translates[i]);
        scene.scale(sections[i], scales[i]);
        printVec(names[i] + "Translate", translates[i]);
        printVec(names[i] + "Scale", scales[i]);
    }
    

    return scene.groupEntities(sections);
}

int main(){
    Scene scene;
    bool test = true;

    //int room = createRoom(scene, 80.f, 8.f, 80.f, 80.f, 0.2f);
    //scene.translate(room, glm::vec4(0.f, -2.f, 0.f, 0.f));

    int room = scene.Tesseract(glm::vec3(1.f), true);
    scene.scale(room, glm::vec4(80.f, 8.f, 80.f, 80.f));

    int xPosWall = wallWithDoor(scene, glm::vec4(1.f, 8.f, 10.f, 10.f), glm::vec4(4.f, 6.f, 4.f, 4.f), glm::vec2(0.f, 0.f));
    int xNegWall = wallWithDoor(scene, glm::vec4(1.f, 8.f, 10.f, 10.f), glm::vec4(4.f, 6.f, 4.f, 4.f), glm::vec2(0.f, 0.f));
    int zPosWall = wallWithDoor(scene, glm::vec4(10.f, 8.f, 1.f, 10.f), glm::vec4(4.f, 6.f, 4.f, 4.f), glm::vec2(0.f, 0.f));
    int zNegWall = wallWithDoor(scene, glm::vec4(10.f, 8.f, 1.f, 10.f), glm::vec4(4.f, 6.f, 4.f, 4.f), glm::vec2(0.f, 0.f));
    int wPosWall = wallWithDoor(scene, glm::vec4(10.f, 8.f, 10.f, 1.f), glm::vec4(4.f, 6.f, 4.f, 4.f), glm::vec2(0.f, 0.f));
    int wNegWall = wallWithDoor(scene, glm::vec4(10.f, 8.f, 10.f, 1.f), glm::vec4(4.f, 6.f, 4.f, 4.f), glm::vec2(0.f, 0.f));

    scene.translate(xPosWall, glm::vec4(5.f, 0.f, 0.f, 0.f));
    scene.translate(xNegWall, glm::vec4(-5.f, 0.f, 0.f, 0.f));
    scene.translate(zPosWall, glm::vec4(0.f, 0.f, 5.f, 0.f));
    scene.translate(zNegWall, glm::vec4(0.f, 0.f, -5.f, 0.f));
    scene.translate(wPosWall, glm::vec4(0.f, 0.f, 0.f, 5.f));
    scene.translate(wNegWall, glm::vec4(0.f, 0.f, 0.f, -5.f));

    int wallTest = scene.groupEntities({ xPosWall, xNegWall, zPosWall, zNegWall, wPosWall, wNegWall });
    
    scene.rotate(wallTest, 2, glm::radians(45.f));
    scene.rotate(wallTest, 5, glm::asin(1.f / sqrt(3.f)));


       

    

    scene.getCamera().setMode("fpv");
    scene.getCamera().setSpawn(glm::vec4(0.f));
    Application app(&scene);
    app.run();
}