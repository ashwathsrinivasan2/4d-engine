#ifndef APPLICATION_H
#define APPLICATION_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <windows.h>
#include <mmsystem.h>


#pragma comment(lib, "winmm.lib")
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
    int keyP;
    int keyI;

    int keyUp;
    int keyDown;
    int keyRight;
    int keyLeft;

    int keySpace;

    bool mousePressed;

    bool collisionsEnabled = true;
    bool muted = false;

    bool rotating = false;
    float rotationTime = 0.f;
    float totalRotationTime = 2.f;

    //audio
    std::wstring backgroundMusicFilename = L"C:\Users\ashwa\VisualStudioProjects\4DEngine\4DEngine\audio\Somewhere Right Now In The Future.wav";


    glm::vec4 lastPosition;

    inline void printVec(glm::vec4 vec) {
        std::cout << "{";
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