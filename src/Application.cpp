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

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(window, 0, 0);
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
    int lastKeyUp = keyUp;
    int lastKeyDown = keyDown;
    int lastKeyRight = keyRight;
    int lastKeyLeft = keyLeft;

    keyW = glfwGetKey(window, GLFW_KEY_W);
    keyA = glfwGetKey(window, GLFW_KEY_A);
    keyS = glfwGetKey(window, GLFW_KEY_S);
    keyD = glfwGetKey(window, GLFW_KEY_D);
    keyQ = glfwGetKey(window, GLFW_KEY_Q);
    keyE = glfwGetKey(window, GLFW_KEY_E);

    keyUp = glfwGetKey(window, GLFW_KEY_UP);
    keyDown = glfwGetKey(window, GLFW_KEY_DOWN);
    keyRight = glfwGetKey(window, GLFW_KEY_RIGHT);
    keyLeft = glfwGetKey(window, GLFW_KEY_LEFT);
    mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;


    

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

    if (lastKeyUp != GLFW_PRESS && keyUp == GLFW_PRESS) {
        scene->getCamera().rotate(5, glm::pi<float>() / 2.f);
    }
    if (lastKeyDown != GLFW_PRESS && keyDown == GLFW_PRESS) {
        scene->getCamera().rotate(5, -glm::pi<float>() / 2.f);
    }
    if (lastKeyRight != GLFW_PRESS && keyRight == GLFW_PRESS) {
        scene->getCamera().rotate(2, glm::pi<float>() / 2.f);
    }
    if (lastKeyLeft != GLFW_PRESS && keyLeft == GLFW_PRESS) {
        scene->getCamera().rotate(2, -glm::pi<float>() / 2.f);
    }

    static bool firstFrame = true;
    if (firstFrame) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos); // discard initial position
        glfwSetCursorPos(window, 0, 0);
        firstFrame = false;
    }
    else {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        glfwSetCursorPos(window, 0, 0);

        if (mousePressed)
        {
            scene->getCamera().rotate(2, (float)xpos * 0.001f);
            scene->getCamera().rotate(5, -(float)ypos * 0.001f);
        }
        else {
            scene->getCamera().rotate(1, (float)xpos * 0.001f);
            scene->getCamera().rotate(3, -(float)ypos * 0.001f);
        }
    }
}