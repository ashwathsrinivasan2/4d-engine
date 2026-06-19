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
    sensitivity = 5.f;
    speed = 8.f;
    fov = glm::radians(120.0f);

    float rotationPlanes[6] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
}


glm::mat4 Camera::getViewMatrix(){
    glm::mat4 view(1.f);

    view[0] = rightVector;
    view[1] = upVector;
    view[2] = -viewVector;
    view[3] = anaVector;

    view = glm::transpose(view);

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
    time *= speed;
    if(orbitMode){
        float distance = glm::length(eye);
        if(distance > 0.001f){
            eye += (time * viewVector);
        }
    } else {
        eye += (time * glm::vec4(viewVector.x, viewVector.y, viewVector.z, viewVector.w));
    }
}
void Camera::moveBackward(float time){
    time *= speed;
    eye -= (time * glm::vec4(viewVector.x, viewVector.y, viewVector.z, viewVector.w));
}
void Camera::moveLeft(float time){
    time *= speed;
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
    time *= speed;
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
    time *= speed;
    if (fpvMode) {
        eye += (time * anaVector);
    }
    else {
        
    }
}

void Camera::moveKata(float time) {
    time *= speed;
    if (fpvMode) {
        eye -= (time * anaVector);
    }
    else {

    }
}

void Camera::orthonormalize() {
    rightVector.y = 0.f;
    anaVector.y = 0.f;
    rightVector = glm::normalize(rightVector);
   
    anaVector = glm::normalize(anaVector);
    
    rightVector = glm::normalize(rightVector - glm::dot(rightVector, anaVector) * anaVector);
    upVector = glm::normalize(upVector
        - glm::dot(upVector, anaVector) * anaVector
        - glm::dot(upVector, rightVector) * rightVector);
    viewVector = glm::normalize(viewVector
        - glm::dot(viewVector, rightVector) * rightVector
        - glm::dot(viewVector, upVector) * upVector
        - glm::dot(viewVector, anaVector) * anaVector);
}

void Camera::rotate(int planeID, float amount) {


    glm::vec4 vA;
    glm::vec4 vB;
    if (planeID == 0 || planeID == 2 || planeID == 4) return;

    float pitch = glm::degrees(asin(glm::clamp(viewVector.y, -1.f, 1.f)));
    switch (planeID) {
    case 0: //xy 
        vA = rightVector;
        vB = upVector;
        break;
    case 1: //xz
        vA = rightVector;
        vB = viewVector;
        vA.y = 0.f;
        vB.y = 0.f;
        break;
    case 2: //xw
        vA = rightVector;
        vB = anaVector;
        vA.y = 0.f;
        vB.y = 0.f;
        break;
    case 3: //yz
        if (pitch > 85.f && amount > 0.f) return;
        if (pitch < -85.f && amount < 0.f) return;
        vA = glm::vec4(0.f, 1.f, 0.f, 0.f);
        vB = viewVector;
        break;
    case 4: //yw
        vA = upVector;
        vB = anaVector;
        break;
    case 5: //zw
        vA = viewVector;
        vB = anaVector;
        vA.y = 0.f;
        vB.y = 0.f;
        vA.x = 0.f;
        vB.x = 0.f;
        break;
    default:
        return;
    }

    vA = glm::normalize(vA);
    vB = glm::normalize(vB);

    Rotor newRotation(vA, vB, amount);

    currRotation.rotate(newRotation);
    rightVector = currRotation.apply(glm::vec4(1.f, 0.f, 0.f, 0.f));
    viewVector = currRotation.apply(glm::vec4(0.f, 0.f, -1.f, 0.f));
    upVector = currRotation.apply(glm::vec4(0.f, 1.f, 0.f, 0.f));
    if(planeID == 2 || planeID == 4 || planeID == 5)
    anaVector = currRotation.apply(glm::vec4(0.f, 0.f, 0.f, 1.f));

    
    orthonormalize();

}

void Camera::initializeOrthogonalRotation(bool posRotation) {
    right = rightVector;
    up = upVector;
    view = viewVector;
    ana = anaVector;

    this->posRotation = posRotation;
    lastInterpolationFactor = 0.f;
}
void Camera::orthogonalZWRotate(float interpolationFactor) {
    
    float radians = smoothstep(0.f, glm::pi<float>() / 2.f, interpolationFactor) - smoothstep(0.f, glm::pi<float>() / 2.f, lastInterpolationFactor);
    lastInterpolationFactor = interpolationFactor;
   

    Rotor newRotation(glm::vec4(0.f, 0.f, -1.f, 0.f), glm::vec4(0.f, 0.f, 0.f, 1.f), radians);

    currRotation.rotate(newRotation);
    rightVector = currRotation.apply(glm::vec4(1.f, 0.f, 0.f, 0.f));
    viewVector = currRotation.apply(glm::vec4(0.f, 0.f, -1.f, 0.f));
    upVector = currRotation.apply(glm::vec4(0.f, 1.f, 0.f, 0.f));
    anaVector = currRotation.apply(glm::vec4(0.f, 0.f, 0.f, 1.f));
    orthonormalize();

    
}
void Camera::finalizeOrthogonalRotation() {
    anaVector = glm::normalize(anaVector);
    int currMax = 0;
    int sign = 1;
    for (int i = 1; i < 4; i++) {
        if (abs(anaVector[i]) > abs(anaVector[currMax])) {
            currMax = i;
            sign = anaVector[i] < 0.f ? -1 : 1;
        }
    }

    anaVector = glm::vec4(0.f);
    anaVector[currMax] = sign;

    orthonormalize();

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
    rightVector = glm::vec4(1.f, 0.f, 0.f, 0.f);
    upVector = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    anaVector = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    for (int i = 0; i < 6; i++) {
        rotationPlanes[i] = 0.f;
    }
}

glm::vec4 Camera::getViewVector(){
    return viewVector;
}