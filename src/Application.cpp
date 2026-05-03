#include "Application.h"

Application::Application(Scene* scene){
    this->scene = scene;
    createWindow();
    renderer.initialize(window, scene);
}

Application::~Application(){
    glfwDestroyWindow(window);
    glfwTerminate();
}

//window helpers
void Application::framebufferResizeCallback(GLFWwindow *window, int width, int height){
    auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
}

void Application::createWindow(){
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Engine", nullptr, nullptr);

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void Application::run(){
    mainLoop();
    vkDeviceWaitIdle(renderer.getLogicalDevice()); //wait for async operations to finish before proceeding
}
void Application::mainLoop(){
    auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    while(!glfwWindowShouldClose(window)){
        startTime = currentTime;
        glfwPollEvents();
        handleInput(time);
        renderer.drawFrame();
        currentTime = std::chrono::high_resolution_clock::now();
        time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    }
}

void Application::handleInput(float time){
    keyW = glfwGetKey(window, GLFW_KEY_W);
    keyA = glfwGetKey(window, GLFW_KEY_A);
    keyS = glfwGetKey(window, GLFW_KEY_S);
    keyD = glfwGetKey(window, GLFW_KEY_D);
    keyQ = glfwGetKey(window, GLFW_KEY_Q);
    keyE = glfwGetKey(window, GLFW_KEY_E);

    if(keyW == GLFW_PRESS){
        scene->getCamera().moveForward(time);
    }
    if(keyA == GLFW_PRESS){
        scene->getCamera().moveLeft(time);
    }
    if(keyS == GLFW_PRESS){
        scene->getCamera().moveBackward(time);
    }
    if(keyD == GLFW_PRESS){
        scene->getCamera().moveRight(time);
    }
    if (keyQ == GLFW_PRESS) {
        scene->getCamera().moveKata(time);
    }
    if (keyE == GLFW_PRESS) {
        scene->getCamera().moveAna(time);
    }

}