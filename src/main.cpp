#define GLM_FORCE_RADIANS
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "Scene.h"
#include "Application.h"

template<typename T>
inline bool contains(std::vector<T> list, T value) {
    for (int i = 0; i < list.size(); i++) {
        if (list[i] == value) {
            return true;
        }
    }
    return false;
}
const std::vector<int> getTwoOnes() {
    std::vector<int> cubeIDs;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                for (int l = 0; l < 3; l++) {
                    int oneCount = 0;
                    oneCount += i == 1;
                    oneCount += j == 1;
                    oneCount += k == 1;
                    oneCount += l == 1;
                    if (oneCount == 2) {
                        int cubeID = i * 27 + j * 9 + k * 3 + l;
                        cubeIDs.push_back(cubeID);
                    }
                }
            }
        }
    }
    return cubeIDs;
}

const std::vector<int> getMengerSponge4D() {
    std::vector<int> cubeIDs;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                for (int l = 0; l < 3; l++) {
                    int oneCount = 0;
                    oneCount += i == 1;
                    oneCount += j == 1;
                    oneCount += k == 1;
                    oneCount += l == 1;
                    if (oneCount < 2) {
                        int cubeID = i * 27 + j * 9 + k * 3 + l;
                        cubeIDs.push_back(cubeID);
                    }
                }
            }
        }
    }
    return cubeIDs;
}

const std::vector<int> testIDs() {
    std::vector<int> cubeIDs;
    for (int i = 0; i < 81; i++) {
        cubeIDs.push_back(i);
    }
    return cubeIDs;
}

void createCubeFractal(Scene& scene, glm::vec4 minCorner, float sideLength, int depth, const std::vector<int>& cubeIDs) {

    if (depth == 0) {
        int unit = scene.Tesseract(glm::vec3(1.f));
        glm::vec4 center = minCorner + glm::vec4(sideLength / 2.f);
        scene.scale(unit, glm::vec4(sideLength));
        scene.translate(unit, center);
        return;
    }
    sideLength /= 3.f;
    depth--;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                for (int l = 0; l < 3; l++) {
                    int cubeID = i * 27 + j * 9 + k * 3 + l;
                    if (contains(cubeIDs, cubeID)) {
                        glm::vec4 newMinCorner = minCorner + glm::vec4(float(i), float(j), float(k), float(l)) * sideLength;
                        createCubeFractal(scene, newMinCorner, sideLength, depth, cubeIDs);
                    }
                }
            }
        }
    }

}


int main(){
    Scene scene;
    bool test = true;

    //createBox(scene, 20.f, 15.f, 20.f, 0.2f);
    //createSpiral(scene, 1000, 2.f, 2);


    //int unit = scene.Tesseract(glm::vec3(1.f));

    createCubeFractal(scene, glm::vec4(-0.5f), 1.f, 3, getTwoOnes());

 

    
    //scene.translate(tesseract, glm::vec4(0.f, 0.f, 0.f, 1.f));

    //scene.Cube(glm::vec3(1.f));
    //scene.rotate(scene.Cube(glm::vec3(1.f)), glm::vec3(1.f), 30.f);



    scene.getCamera().setMode("fpv");
    scene.getCamera().setSpawn(glm::vec4(0.f));
    Application app(&scene);
    app.run();
}