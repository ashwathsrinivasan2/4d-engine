#ifndef CAMERA
#define CAMERA

#include <glm/glm.hpp>
#include "string"

//creates view and projection matrices
class Camera{

    private:
    glm::vec3 eye;
    glm::vec3 viewVector;
    glm::vec3 upVector;
    glm::vec2 lastMousePos;
    glm::vec3 spawn = glm::vec3(0.f);
    float fov;
    float sensitivity;

    float maxPitch = 89.0f;
    float minPitch = -89.0f;
    float currPitch = 0.0f;

    //modes
    bool fpvMode = false;
    bool orbitMode = false;


    float currYaw = 0.0f;

    glm::vec3 rightVector();
    
    public:
    Camera();
    glm::mat4 getViewMatrix();
    float getFOV();
    glm::vec3 getPosition();
    glm::vec3 getViewVector();
    void setSpawn(glm::vec3 origin){eye = origin; spawn = origin;}
    glm::vec3 getSpawn(){return spawn;}

    void setMode(std::string);
    
    void moveForward(float);
    void moveBackward(float);
    void moveLeft(float);
    void moveRight(float);
    void zoom(float);
    void setSensitivity(float);
    void reset();

    void mouseLook(int, int);
};

#endif