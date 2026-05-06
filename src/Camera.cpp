#define GLM_ENABLE_EXPERIMENTAL
#include "Camera.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/constants.hpp"
#include <iostream>

Camera::Camera(){
    eye = glm::vec4(0.f);
    viewVector = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    upVector = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    rightVector = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    anaVector = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    lastMousePos = glm::vec2(0.0f, 0.0f);
    sensitivity = 0.5f;
    fov = glm::radians(90.0f);
}


glm::mat4 Camera::getViewMatrix(){
    glm::mat4 view(1.f);

    view[0] = rightVector;
    view[1] = upVector;
    view[2] = -viewVector;
    view[3] = anaVector;

    

    return view;
}

float Camera::getFOV(){
    return fov;
}

glm::vec4 Camera::getPosition(){
    return eye;
}

void Camera::setMode(std::string mode){
    if(mode == "fpv"){
        fpvMode = true;
        orbitMode = false;
    } else if (mode == "orbit"){
        orbitMode = true;
        fpvMode = false;
    } else {
        setMode("fpv");
    }
}

void Camera::moveForward(float time){
    time *= sensitivity;
    if(orbitMode){
        float distance = glm::length(eye);
        if(distance > 0.001f){
            eye += (time * viewVector);
        }
    } else {
        eye += (time * viewVector);
    }
}
void Camera::moveBackward(float time){
    time *= sensitivity;
    eye -= (time * viewVector);
}
void Camera::moveLeft(float time){
    time *= sensitivity;
    if(fpvMode){
        eye -= (time * rightVector);
    } else {
        glm::vec4 center(0.f);
        float radius = glm::length(center - eye);
        if(radius > 0){
            eye -= (time * rightVector);
            float updatedCenterDistance = pow((pow (glm::length ( (time * rightVector) ), 2) + radius * radius), 0.5f);
            glm::vec4 eyeToCenter = center - eye;
            eye = eye + ( (updatedCenterDistance - radius) * eyeToCenter / updatedCenterDistance);
            viewVector = glm::normalize(center - eye);
        }
    }
}
void Camera::moveRight(float time){  
    time *= sensitivity;
    if(fpvMode){
        eye += (time * rightVector);
    } else {
        glm::vec4 center(0.f);
        float radius = glm::length(center - eye);
        if(radius > 0){
            eye += (time * rightVector);
            float updatedCenterDistance = pow((pow (glm::length ( (time * rightVector) ), 2) + radius * radius), 0.5f);
            glm::vec4 eyeToCenter = center - eye;
            eye = eye + ( (updatedCenterDistance - radius) * eyeToCenter / updatedCenterDistance);
            viewVector = glm::normalize(center - eye);
        }
    }
}

void Camera::moveAna(float time) {
    time *= sensitivity;
    if (fpvMode) {
        eye += (time * anaVector);
    }
    else {
        
    }
}

void Camera::moveKata(float time) {
    time *= sensitivity;
    if (fpvMode) {
        eye -= (time * anaVector);
    }
    else {

    }
}

void Camera::zoom(float newFOV){
    fov = newFOV;
}

void Camera::setSensitivity(float newSens){
    sensitivity = newSens;
}

void Camera::reset(){
    eye = spawn;
    viewVector = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    upVector = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    anaVector = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    currPitch = 0.0f;
    currYaw = 0.0f;
}

glm::vec4 Camera::getViewVector(){
    return viewVector;
}