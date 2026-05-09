#ifndef ROTOR_H
#define ROTOR_H

#include <glm/glm.hpp>

class Rotor{

    float s;
    float xy;
    float xz;
    float xw;
    float yz;
    float yw;
    float zw;
    float xyzw;

    void normalize();

    public:
    Rotor(float a = 1.f, float xy = 0.f, float xz = 0.f, float xw = 0.f, float yz = 0.f, float yw = 0.f, float zw = 0.f, float xyzw = 0.f);
    Rotor(int planeID, float radians);

    void setScalar(float val)   {s = val;}
    void setXY(float val)       {xy = val;}
    void setXZ(float val)       {xz = val;}
    void setXW(float val)       {xw = val;}
    void setYZ(float val)       {yz = val;}
    void setYW(float val)       {yw = val;}
    void setZW(float val)       {zw = val;}
    void setXYZW(float val) { xyzw = val; }

    float getScalar() { return s; }
    float getXY() { return xy; }
    float getXZ() { return xz; }
    float getXW() { return xw; }
    float getYZ() { return yz; }
    float getYW() { return yw; }
    float getZW() { return zw; }
    float getXYZW() { return xyzw; }

    glm::vec4 apply(glm::vec4);
    void rotate(Rotor& a);

    glm::mat4 toMatrix();

    void reset();
};

#endif