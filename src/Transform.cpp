#include "Transform.h"
#include <iostream>

Transform::Transform(){
    scaleMat = glm::mat4(1.0f);
    rotateMat = glm::mat4(1.0f);
    translateMat = glm::mat4(1.0f);

    currScale = glm::vec3(1.0f);
    currTranslate = glm::vec3(0.0f);
    currRotate = glm::quat(1.f, 0.f, 0.f, 0.f);
}

void Transform::scale(glm::vec3 scaleInput){
    currScale.x *= scaleInput.x;
    currScale.y *= scaleInput.y;
    currScale.z *= scaleInput.z;
    scaleMat = glm::scale(glm::mat4(1.f), currScale);
}
void Transform::rotate(glm::vec3 axis, float radians){
    axis = glm::normalize(axis);
    currRotate = glm::angleAxis(radians, axis) * currRotate;
    rotateMat = glm::toMat4(currRotate);
}
void Transform::translate(glm::vec3 translation){
    currTranslate += translation;
    translateMat = glm::translate(glm::mat4(1.f), currTranslate);
}

glm::mat4 Transform::getModelMatrix(){
    return translateMat * rotateMat * scaleMat;
}