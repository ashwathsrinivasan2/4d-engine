#ifndef APPLICATION_H
#define APPLICATION_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Scene.h"
#include "Renderer.h"
#include "WorldGenerator.h"

class Application{

    const unsigned int WIDTH = 1920;
    const unsigned int HEIGHT = 1080;

    GLFWwindow* window;
    bool framebufferResized;

    Renderer renderer;
    Scene* scene;
    WorldGenerator worldGen;

    void mainLoop();
    void handleInput(float);

    static void framebufferResizeCallback(GLFWwindow*, int, int);

    //input states
    int keyW;
    int keyA;
    int keyS;
    int keyD;
    int keyQ;
    int keyE;

    int keyUp;
    int keyDown;
    int keyRight;
    int keyLeft;

    int keySpace;

    bool mousePressed;

    glm::vec4 lastPosition;

    inline void printVec(std::string name, glm::vec4 vec) {
        std::cout << name << ": {";
        for (int i = 0; i < 4; i++) {
            std::cout << vec[i];
            if (i != 4) std::cout << " ";
        }
        std::cout << "}" << std::endl;
    }

    public:

    Application(Scene* scene, WorldGenerator worldGen);
    ~Application();
    void createWindow();
    void run();
};

#endif