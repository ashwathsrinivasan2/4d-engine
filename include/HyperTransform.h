#ifndef HYPERTRANSFORM_H
#define HYPERTRANSFORM_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Rotor.h"

class HyperTransform{

    glm::vec4 currTranslation;
    glm::vec4 currScale;

    glm::mat4 scaleMat;

    Rotor currRotation;

    /*glm::vec4 project(glm::vec4 a, glm::vec4 b);
    Rotor geoProd(glm::vec4 a, glm::vec4 b);
    bool linearlyIndependent(glm::vec4 v, glm::vec4 a = glm::vec4(0.f), glm::vec4 b = glm::vec4(0.f), glm::vec4 c = glm::vec4(0.f));*/

    public:
    HyperTransform();

    void rotate(float, int, int, int, int);
    void translate(glm::vec4);
    void scale(glm::vec4);

    glm::mat4 getModelMatrix();
    glm::vec4 getTranslate(){return currTranslation;}

    void getRotor(float& s, float& xy, float& xz, float& xw, float& yz, float& yw, float& zw);

    void reset();

    void test(){
        currRotation.setScalar(0.8409);
        currRotation.setXW(0.3827);
        currRotation.setYW(0.3015);
        currRotation.setZW(0.3827);
        currRotation.normalize();
    }

};

#endif