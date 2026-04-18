#include "HyperTransform.h"
#include <cmath>

HyperTransform::HyperTransform(){
    currScale = glm::vec4(1.f, 1.f, 1.f, 1.f);
    currTranslation = glm::vec4(0.f, 0.f, 0.f, 0.f);
    scaleMat = glm::mat4(1.f);
    currRotation.setScalar(1.f);
}

void HyperTransform::translate(glm::vec4 translation){
    currTranslation += translation;
}

void HyperTransform::scale(glm::vec4 scalar){
    currScale *= scalar;
    scaleMat = glm::mat4(
        glm::vec4(currScale.x, 0.f, 0.f, 0.f),
        glm::vec4(0.f, currScale.y, 0.f, 0.f),
        glm::vec4(0.f, 0.f, currScale.z, 0.f),
        glm::vec4(0.f, 0.f, 0.f, currScale.w)
    );
}

void HyperTransform::rotate(float radians, int planeX, int planeY, int planeZ, int planeW){

    //check if valid plane
    if(planeX != 0){
        planeX = 1;
    }
    if(planeY != 0){
        planeY = 1;
    }
    if(planeZ != 0){
        planeZ = 1;
    }
    if(planeW != 0){
        planeW = 1;
    }
    if(planeX + planeY + planeZ + planeW != 2) return;


    float cos = std::cosf(radians / 2.f);
    float sin = std::sinf(radians / 2.f);
    Rotor newRotation(1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f);

    newRotation.setScalar(cos);
    if(planeX + planeY == 2){
        newRotation.setXY(sin);
    } else if(planeX + planeZ == 2){
        newRotation.setXZ(sin);
    } else if(planeX + planeW == 2){
        newRotation.setXW(sin);
    } else if(planeY + planeZ == 2){
        newRotation.setYZ(sin);
    } else if(planeY + planeW == 2){
        newRotation.setYW(sin);
    } else {
        newRotation.setZW(sin);
    }
    newRotation.normalize();

    currRotation = newRotation * currRotation;
}

glm::mat4 HyperTransform::getModelMatrix(){
    glm::mat4 rotationMat = currRotation.toMatrix();
    glm::mat4 model = rotationMat * scaleMat;

    return model;
}

void HyperTransform::reset(){
    currTranslation = glm::vec4(0.f);
    currScale = glm::vec4(1.f);
    currRotation.reset();
}

void HyperTransform::getRotor(float& s, float& xy, float& xz, float& xw, float& yz, float& yw, float& zw){
    s = currRotation.getComponent(0);
    xy = currRotation.getComponent(1);
    xz = currRotation.getComponent(2);
    xw = currRotation.getComponent(3);
    yz = currRotation.getComponent(4);
    yw = currRotation.getComponent(5);
    zw = currRotation.getComponent(6);
}