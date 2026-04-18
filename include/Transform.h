#ifndef TRANSFORM_H
#define TRANSFORM_H

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

//creates model matrices (scale, translate, rotate)
class Transform{
    glm::mat4 scaleMat;
    glm::mat4 rotateMat;
    glm::mat4 translateMat;

    glm::vec3 currScale;
    glm::vec3 currTranslate;
    glm::quat currRotate;

    public:
    Transform();

    void scale(glm::vec3);
    void rotate(glm::vec3, float);
    void translate(glm::vec3);

    glm::mat4 getModelMatrix();
};

#endif