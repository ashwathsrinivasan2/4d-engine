#define GLM_FORCE_RADIANS
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "Scene.h"
#include "Entity.h"
#include "Application.h"

void createBox(Scene& scene, float w, float h, float l, float t){
    std::vector<int> walls;
    for(int i = 0; i < 6; i++){
        walls.push_back(scene.Cube(glm::vec3(1.f, 0.f, 0.f)));
    }
    
    scene.scale(walls[0], glm::vec3(t, h, l));
    scene.scale(walls[1], glm::vec3(t, h, l));
    scene.scale(walls[2], glm::vec3(w, t, l));
    scene.scale(walls[3], glm::vec3(w, t, l));
    scene.scale(walls[4], glm::vec3(w, h, t));
    scene.scale(walls[5], glm::vec3(w, h, t));

    float xTranslate = w / 2.f + t / 2.f;
    float yTranslate = h / 2.f + t / 2.f;
    float zTranslate = l / 2.f + t / 2.f;

    scene.translate(walls[0], glm::vec3(xTranslate, 0.f, 0.f));
    scene.translate(walls[1], glm::vec3(-xTranslate, 0.f, 0.f));
    scene.translate(walls[2], glm::vec3(0.f, yTranslate, 0.f));
    scene.translate(walls[3], glm::vec3(0.f, -yTranslate, 0.f));
    scene.translate(walls[4], glm::vec3(0.f, 0.f, zTranslate));
    scene.translate(walls[5], glm::vec3(0.f, 0.f, -zTranslate));

    for(int i = 0; i < 6; i++){
        scene.translate(walls[i], glm::vec3(0.f, 5.f, 0.f));
    }
}

void createSpiral(Scene& scene, int detail, float size, int numTwists){
    for(int i = 0; i < detail; i++){
        int newCube = scene.Cube(glm::vec3(1.f));
        scene.scale(newCube, glm::vec3(1.f, size / (float)detail, 1.f));
        scene.rotate(newCube, glm::vec3(0.f, 1.f, 0.f), (float)numTwists * (glm::pi<float>() / 2.f) / (float)detail * (float)i);
        scene.translate(newCube, glm::vec3(0.f, size / (2.f * (float)detail) - (size / 2.f) + (float)i * (size / (float)detail), 0.f));
    }
}


int main(){
    Scene scene;
    Application app(&scene);

    //createBox(scene, 20.f, 15.f, 20.f, 0.2f);
    //createSpiral(scene, 1000, 2.f, 2);

    //scene.Tesseract(glm::vec3(1.f));

    //scene.Cube(glm::vec3(1.f));
    scene.rotate(scene.Cube(glm::vec3(1.f)), glm::vec3(1.f), 30.f);



    scene.getCamera().setMode("orbit");
    app.run();
}