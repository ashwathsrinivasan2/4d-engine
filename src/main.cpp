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

int createRoom(Scene& scene, float width, float height, float length, float depth, float thickness) {
    std::vector<int> walls;

    for (int i = 0; i < 8; i++) {
        walls.push_back(scene.Tesseract(glm::vec3(1.f)));
    }

    glm::vec4 dim(width, height, length, depth);
    for (int i = 0; i < 8; i++) {
        glm::vec4 scale = dim;
        scale[i / 2] = thickness;
        glm::vec4 translate(0.f);
        translate[i / 2] = (float)((i % 2) * 2 - 1) * (dim[i / 2] / 2.f + thickness / 2.f);
        scene.scale(walls[i], scale);
        scene.translate(walls[i], translate);
    }   

    return scene.groupEntities(walls);
}

int main(){
    Scene scene;
    bool test = true;

    //int room = createRoom(scene, 80.f, 8.f, 80.f, 80.f, 0.2f);
    //scene.translate(room, glm::vec4(0.f, -2.f, 0.f, 0.f));

    int room = scene.Tesseract(glm::vec3(1.f), true);
    scene.scale(room, glm::vec4(80.f, 8.f, 80.f, 80.f));

    scene.getCamera().setMode("fpv");
    scene.getCamera().setSpawn(glm::vec4(0.f));
    Application app(&scene);
    app.run();
}