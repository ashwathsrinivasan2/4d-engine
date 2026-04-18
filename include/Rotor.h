#ifndef ROTOR_H
#define ROTOR_H

#include <glm/glm.hpp>

class Rotor{

    //a, xy, xz, xw, yz, yw, zw
    float components[7];

    //0 for omitted terms
    //1-7 for 0-6 indices
    //sign determines whether to add or subtract term
    int termLookupTable[7][7] = 
    {
        1, 2, 3, 4, 5, 6, 7,
        2, -1, -5, -6, 3, 4, 0,
        3, 5, -1, -7, -2, 0, 4,
        4, 6, 7, -1, 0, -2, -3,
        5, -3, 2, 0, -1, -7, 6,
        6, -4, 0, 2, 7, -1, -5,
        7, 0, -4, 3, -6, 5, -1
    };

    glm::vec4 rotateVec(glm::vec4);
    Rotor getReverse();
    glm::vec4 vecTimesRotor(glm::vec4, Rotor);
    glm::vec4 rotorTimesVec(glm::vec4, Rotor);
    bool checkNormalized();

    public:
    Rotor(float a = 1.f, float xy = 0.f, float xz = 0.f, float xw = 0.f, float yz = 0.f, float yw = 0.f, float zw = 0.f);
    float getComponent(int index) {return components[index];}

    void setScalar(float val)   {components[0] = val;}
    void setXY(float val)       {components[1] = val;}
    void setXZ(float val)       {components[2] = val;}
    void setXW(float val)       {components[3] = val;}
    void setYZ(float val)       {components[4] = val;}
    void setYW(float val)       {components[5] = val;}
    void setZW(float val)       {components[6] = val;}

    glm::mat4 toMatrix();

    void normalize();

    Rotor sqrtRotor();

    void reset();

    Rotor operator*(Rotor& other);
};

#endif