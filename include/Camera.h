#ifndef CAMERA
#define CAMERA

#include <glm/glm.hpp>
#include "string"
#include "Rotor.h"
#include <iostream>
//creates view and projection matrices
class Camera{

    private:
    glm::vec4 eye;
    glm::vec4 viewVector;
    glm::vec4 rightVector;
    glm::vec4 upVector;
    glm::vec4 anaVector;
    glm::vec2 lastMousePos;
    glm::vec4 spawn = glm::vec4(0.f);
    float fov;
    float sensitivity;
    float speed;

    float maxPitch = 89.0f;
    float minPitch = -89.0f;

    float rotationPlanes[6];

    //modes
    bool fpvMode = false;
    bool orbitMode = false;

    Rotor currRotation;

    //orthogonal rotations
    glm::vec4 right;
    glm::vec4 up;
    glm::vec4 view;
    glm::vec4 ana;
    bool posRotation;
    float lastInterpolationFactor = 0.f;



    float currYaw = 0.0f;

    void orthonormalize();

    inline float smoothstep(float start, float end, float interpolationFactor) {
        if (interpolationFactor <= 0.f) return start;
        if (interpolationFactor >= 1.f) return end;
        float smoothStepFactor = 3 * interpolationFactor * interpolationFactor - 2 * interpolationFactor * interpolationFactor * interpolationFactor;
        return (1.f - smoothStepFactor) * start + smoothStepFactor * end;
    }

    inline void printVec(std::string name, glm::vec4 vec) {
        std::cout << name << ": {";
        for (int i = 0; i < 4; i++) {
            std::cout << vec[i];
            if (i != 4) std::cout << " ";
        }
        std::cout << "}" << std::endl;
    }

    void printVectors() {
        printVec("right", rightVector);
        printVec("up", upVector);
        printVec("view", viewVector);
        printVec("ana", anaVector);
        std::cout << std::endl;
    }
    
    public:
    Camera();
    glm::mat4 getViewMatrix();
    float getFOV();
    glm::vec4 getPosition();
    glm::vec4 getViewVector();
    void setSpawn(glm::vec4 origin){eye = origin; spawn = origin;}
    glm::vec4 getSpawn(){return spawn;}

    void setPosition(glm::vec4 pos) { eye = pos; }

    void setMode(std::string);
    
    void moveForward(float);
    void moveBackward(float);
    void moveLeft(float);
    void moveRight(float);
    void moveAna(float);
    void moveKata(float);

    void rotate(int, float);
    
    void initializeOrthogonalRotation(bool);
    void orthogonalZWRotate(float);
    void finalizeOrthogonalRotation();
    

    void zoom(float);
    void setSensitivity(float);
    void reset();

    glm::vec4 getRight() { return rightVector; }
    glm::vec4 getUp() { return upVector; }
    glm::vec4 getView() { return viewVector; }
    glm::vec4 getAna() { return anaVector; }
};

#endif