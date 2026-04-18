#include "Rotor.h"
#include <cmath>
#include <iostream>

Rotor::Rotor(float a, float xy, float xz, float xw, float yz, float yw, float zw){
    components[0] = a;
    components[1] = xy;
    components[2] = xz;
    components[3] = xw;
    components[4] = yz;
    components[5] = yw;
    components[6] = zw;
    normalize();
}

Rotor Rotor::getReverse(){
    Rotor reverse(
        components[0],
        -components[1],
        -components[2],
        -components[3],
        -components[4],
        -components[5],
        -components[6]
    );

    return reverse;
}

bool Rotor::checkNormalized(){
    float length = 0.f;
    for(int i = 0; i < 7; i++){
        length += components[i] * components[i];
    }
    float error = std::sqrtf(length) - 1.f;
    return error < 0.0000001f && error > -0.0000001f;
}

glm::vec4 Rotor::rotateVec(glm::vec4 vec){
    return vecTimesRotor(rotorTimesVec(vec, *this), getReverse());
}

Rotor Rotor::operator*(Rotor& other){
    int component;
    int sign;
    float product;
    float newComponents[7] = {0.f};

    for(int i = 0; i < 7; i++){
        for(int j = 0; j < 7; j++){
            sign = 1;
            component = termLookupTable[i][j];
            if(component != 0){
                if(component < 0){
                    sign = -1;
                    component *= -1;
                }
                component--;
                product = sign * components[i] * other.getComponent(j);
                newComponents[component] += product;
            }
        }
    }

    Rotor result(newComponents[0], newComponents[1], newComponents[2], newComponents[3], newComponents[4], newComponents[5], newComponents[6]);
    return result;
}

Rotor Rotor::sqrtRotor(){
    float a = components[0];
    float b = components[1];
    float c = components[2];
    float d = components[3];
    float e = components[4];
    float f = components[5];
    float g = components[6];

    float h, i, j, k, l, m, n;


    h = sqrt(a * a + sqrt((a * a + b * b + c * c + d * d + e * e + f * f + g * g) / 2));
    i = b / (2 * h);
    j = c / (2 * h);
    k = d / (2 * h);
    l = e / (2 * h);
    m = f / (2 * h);
    n = g / (2 * h);

    Rotor result(h, i, j, k, l, m, n);
    return result;
}

void Rotor::normalize(){
    float length = 0.f;
    for(int i = 0; i < 7; i++){
        length += components[i] * components[i];
    }
    if(length == 0.f) {
        reset();
    } else {
        length = std::sqrtf(length);

        for(int i = 0; i < 7; i++){
            components[i] /= length;
        }
    }
}

glm::vec4 Rotor::vecTimesRotor(glm::vec4 v, Rotor r){
    float a = v.x;
    float b = v.y;
    float c = v.z;
    float d = v.w;

    float s = r.getComponent(0);
    float e = r.getComponent(1);
    float f = r.getComponent(2);
    float g = r.getComponent(3);
    float h = r.getComponent(4);
    float i = r.getComponent(5);
    float j = r.getComponent(6);

    float x = a * s - b * e - c * f - d * g;
    float y = a * e + b * s - c * h - d * i;
    float z = a * f + b * h + c * s - d * j;
    float w = a * g + b * i + c * j + d * s;

    glm::vec4 result = glm::vec4(x, y, z, w);
    return result;
}

glm::vec4 Rotor::rotorTimesVec(glm::vec4 v, Rotor r){
    float a = v.x;
    float b = v.y;
    float c = v.z;
    float d = v.w;

    float s = r.getComponent(0);
    float e = r.getComponent(1);
    float f = r.getComponent(2);
    float g = r.getComponent(3);
    float h = r.getComponent(4);
    float i = r.getComponent(5);
    float j = r.getComponent(6);

    float x = a * s + b * e + c * f + d * g;
    float y = -a * e + b * s + c * h + d * i;
    float z = -a * f - b * h + c * s + d * j;
    float w = -a * g - b * i - c * j + d * s;

    glm::vec4 result = glm::vec4(x, y, z, w);
    return result;
}

glm::mat4 Rotor::toMatrix(){
    glm::vec4 x(1.f, 0.f, 0.f, 0.f);
    glm::vec4 y(0.f, 1.f, 0.f, 0.f);
    glm::vec4 z(0.f, 0.f, 1.f, 0.f);
    glm::vec4 w(0.f, 0.f, 0.f, 1.f);

    glm::mat4 mat(1.f);
    mat[0] = glm::normalize(rotateVec(x));
    mat[1] = glm::normalize(rotateVec(y));
    mat[2] = glm::normalize(rotateVec(z));
    mat[3] = glm::normalize(rotateVec(w));

    return mat;
}

void Rotor::reset(){
    for(int i = 0;i < 7; i++){
        components[i] = 0.f;
    }
    components[0] = 1.f;
}