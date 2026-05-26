#ifndef APPLICATION_H
#define APPLICATION_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Scene.h"
#include "Renderer.h"

class Application{

    const unsigned int WIDTH = 1920;
    const unsigned int HEIGHT = 1080;

    GLFWwindow* window;
    bool framebufferResized;

    Renderer renderer;
    Scene* scene;

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

    bool mousePressed;

    public:

    Application(Scene* scene);
    ~Application();
    void createWindow();
    void run();
};

#endif