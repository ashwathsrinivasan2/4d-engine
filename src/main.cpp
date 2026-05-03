#define GLM_FORCE_RADIANS
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "Scene.h"
#include "Application.h"


int main(){
    Scene scene;

    //createBox(scene, 20.f, 15.f, 20.f, 0.2f);
    //createSpiral(scene, 1000, 2.f, 2);

    int tesseract = scene.Tesseract(glm::vec3(1.f, 1.f, 0.f));

    //scene.Cube(glm::vec3(1.f));
    //scene.rotate(scene.Cube(glm::vec3(1.f)), glm::vec3(1.f), 30.f);



    scene.getCamera().setMode("orbit");
    scene.getCamera().setSpawn(glm::vec4(0.f, 0.f, 0.f, 0.f));
    Application app(&scene);
    app.run();
}