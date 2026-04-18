#define GLM_ENABLE_EXPERIMENTAL
#include "Camera.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include <iostream>

Camera::Camera(){
    eye = glm::vec3(0.f);
    viewVector = glm::vec3(0.0f, 0.0f, -1.0f);
    upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    lastMousePos = glm::vec2(0.0f, 0.0f);
    sensitivity = 10.f;
    fov = glm::radians(90.0f);
}


glm::mat4 Camera::getViewMatrix(){
    return glm::lookAt(eye, eye + viewVector, upVector);
}

float Camera::getFOV(){
    return fov;
}

glm::vec3 Camera::getPosition(){
    return eye;
}

void Camera::mouseLook(int mouseX, int mouseY){
    if(fpvMode){
        glm::vec2 currMousePos = glm::vec2(mouseX, mouseY);

        static bool firstLook = true;
        if(firstLook){
            lastMousePos = currMousePos;
            firstLook = false;
        }

        glm::vec2 mouseDelta = lastMousePos - currMousePos;
        currYaw += (float)mouseDelta[0];

        if((currPitch <= maxPitch && mouseDelta[1] > 0) || (currPitch >= minPitch && mouseDelta[1] < 0))
            currPitch += (float)mouseDelta[1];

        viewVector = glm::rotate(glm::vec3(0.0f, 0.0f, -1.0f), glm::radians(currYaw), upVector);
        viewVector = glm::rotate(viewVector, glm::radians(currPitch), rightVector());
        upVector = glm::rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(currPitch), rightVector());

        lastMousePos = currMousePos;
    }
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
        eye -= (sensitivity * rightVector());
    } else {
        glm::vec3 center(0.f);
        float radius = glm::length(center - eye);
        if(radius > 0){
            eye -= (time * rightVector());
            float updatedCenterDistance = pow((pow (glm::length ( (time * rightVector()) ), 2) + radius * radius), 0.5f);
            glm::vec3 eyeToCenter = center - eye;
            eye = eye + ( (updatedCenterDistance - radius) * eyeToCenter / updatedCenterDistance);
            viewVector = glm::normalize(center - eye);
        }
    }
}
void Camera::moveRight(float time){  
    time *= sensitivity;
    if(fpvMode){
        eye += (time * rightVector());
    } else {
        glm::vec3 center(0.f);
        float radius = glm::length(center - eye);
        if(radius > 0){
            eye += (time * rightVector());
            float updatedCenterDistance = pow((pow (glm::length ( (time * rightVector()) ), 2) + radius * radius), 0.5f);
            glm::vec3 eyeToCenter = center - eye;
            eye = eye + ( (updatedCenterDistance - radius) * eyeToCenter / updatedCenterDistance);
            viewVector = glm::normalize(center - eye);
        }
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
    viewVector = glm::vec3(0.0f, 0.0f, -1.0f);
    upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    currPitch = 0.0f;
    currYaw = 0.0f;
}

glm::vec3 Camera::rightVector(){
    return glm::normalize(glm::cross(viewVector, upVector));
}

glm::vec3 Camera::getViewVector(){
    return viewVector;
}