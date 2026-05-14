#define GLM_FORCE_RADIANS
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include <cstdlib>
#include <ctime>

#include "Scene.h"
#include "Application.h"
#include "Rotor.h"
#include "WorldGenerator.h"


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

int main(){
    Scene scene;
    srand(time(NULL));

    WorldGenerator generator(&scene);
    generator.randGenerate();

    scene.Cube();

    //create random rooms



    /*
    
    int xPosWall = wallWithDoor(scene, glm::vec4(1.f, 8.f, 10.f, 10.f), glm::vec2(0.f, 0.f));
    int xNegWall = wallWithDoor(scene, glm::vec4(1.f, 8.f, 10.f, 10.f), glm::vec2(0.f, 0.f));
    int zPosWall = wallWithDoor(scene, glm::vec4(10.f, 8.f, 1.f, 10.f), glm::vec2(0.f, 0.f));
    int zNegWall = wallWithDoor(scene, glm::vec4(10.f, 8.f, 1.f, 10.f), glm::vec2(0.f, 0.f));
    int wPosWall = wallWithDoor(scene, glm::vec4(10.f, 8.f, 10.f, 1.f), glm::vec2(0.f, 0.f));
    int wNegWall = wallWithDoor(scene, glm::vec4(10.f, 8.f, 10.f, 1.f), glm::vec2(0.f, 0.f));
    

    int xPosCorridor = corridor(scene, 20.f, 0);
    int xNegCorridor = corridor(scene, 20.f, 0);
    int zPosCorridor = corridor(scene, 20.f, 2);
    int zNegCorridor = corridor(scene, 20.f, 2);
    int wPosCorridor = corridor(scene, 20.f, 3);
    int wNegCorridor = corridor(scene, 20.f, 3);

    
    scene.translate(xPosWall, glm::vec4(5.f, 0.f, 0.f, 0.f));
    scene.translate(xNegWall, glm::vec4(-5.f, 0.f, 0.f, 0.f));
    scene.translate(zPosWall, glm::vec4(0.f, 0.f, 5.f, 0.f));
    scene.translate(zNegWall, glm::vec4(0.f, 0.f, -5.f, 0.f));
    scene.translate(wPosWall, glm::vec4(0.f, 0.f, 0.f, 5.f));
    scene.translate(wNegWall, glm::vec4(0.f, 0.f, 0.f, -5.f));
    
    float thickness = 1.f / 2.f;

    scene.translate(xPosCorridor, glm::vec4(15.f + thickness, 0.f, 0.f, 0.f));
    scene.translate(xNegCorridor, glm::vec4(-15.f - thickness, 0.f, 0.f, 0.f));
    scene.translate(zPosCorridor, glm::vec4(0.f, 0.f, 15.f + thickness, 0.f));
    scene.translate(zNegCorridor, glm::vec4(0.f, 0.f, -15.f  - thickness, 0.f));
    scene.translate(wPosCorridor, glm::vec4(0.f, 0.f, 0.f, 15.f + thickness));
    scene.translate(wNegCorridor, glm::vec4(0.f, 0.f, 0.f, -15.f - thickness));



    int wallTest = scene.groupEntities({ xPosWall, xNegWall, zPosWall, zNegWall, wPosWall, wNegWall });
    int corridorTest = scene.groupEntities({ xPosCorridor, xNegCorridor, zPosCorridor, zNegCorridor, wPosCorridor, wNegCorridor });
    int fullTest = scene.groupEntities({ wallTest, corridorTest });

    scene.translate(corridorTest, glm::vec4(0.f, -1.f, 0.f, 0.f));
    */
    
    //scene.rotate(fullTest, 2, glm::radians(45.f));
    //scene.rotate(fullTest, 5, glm::asin(1.f / sqrt(3.f)));
    
    scene.getCamera().setMode("fpv");
    scene.getCamera().setSpawn(glm::vec4(0.f));
    Application app(&scene);
    app.run();
}