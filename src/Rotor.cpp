#include "Rotor.h"
#include <cmath>

Rotor::Rotor(float s, float xy, float xz, float xw, float yz, float yw, float zw, float xyzw) {
	this->s = s;
	this->xy = xy;
	this->xz = xz;
	this->xw = xw;
	this->yz = yz;
	this->yw = yw;
	this->zw = zw;
	this->xyzw = xyzw;
}

Rotor::Rotor(int planeID, float radians) {
	s = cos(radians / 2.f);
	xy = 0.f;
	xz = 0.f;
	xw = 0.f;
	yz = 0.f;
	yw = 0.f;
	zw = 0.f;
	xyzw = 0.f;

	float bVal = sin(radians / 2.f);
	switch (planeID) {
	case 0:
		xy = bVal;
		break;
	case 1:
		xz = bVal;
		break;
	case 2:
		xw = bVal;
		break;
	case 3:
		yz = bVal;
		break;
	case 4:
		yw = bVal;
		break;
	case 5:
		zw = bVal;
		break;
	default:
		s = 1.f;
	}
}

void Rotor::normalize() {
	float magnitude = sqrt(s * s + xy * xy + xz * xz + xw * xw + yz * yz + yw * yw + zw * zw + xyzw * xyzw);
	s /= magnitude;
	xy /= magnitude;
	xz /= magnitude;
	xw /= magnitude;
	yz /= magnitude;
	yw /= magnitude;
	zw /= magnitude;
	xyzw /= magnitude;
}

glm::vec4 Rotor::apply(glm::vec4 v) {
	//(r.s, r.xy, r.xz, r.xw, r.yz, r.yw, r.zw, r.xyzw) * (v.x, v.y, v.z, v.w) = 
	float mx = s * v.x + xy * v.y + xz * v.z + xw * v.w;
	float my = s * v.y - xy * v.x + yz * v.z + yw * v.w;
	float mz = s * v.z - xz * v.x - yz * v.y + zw * v.w;
	float mw = s * v.w - xw * v.x - yw * v.y - zw * v.z;
	float mxyz = xy * v.z - xz * v.y + yz * v.x + xyzw * v.w;
	float mxyw = xy * v.w - xw * v.y + yw * v.x - xyzw * v.z;
	float mxzw = xz * v.w - xw * v.z + zw * v.x + xyzw * v.y;
	float myzw = yz * v.w - yw * v.z + zw * v.y - xyzw * v.x;

	//(mx, my, mz, mw, mxyz, mxyw, mxzw, myzw) * (r.s, -r.xy, -r.xz, -r.xw, -r.yz, -r.yw, -r.zw, r.xyzw)
	float x = mx * s + my * xy + mz * xz + mw * xw + mxyz * yz + mxyw * yw + mxzw * zw + myzw * xyzw;
	float y = -mx * xy + my * s + mz * yz + mw * yw - mxyz * xz - mxyw * xw - mxzw * xyzw + myzw * zw;
	float z = -mx * xz - my * yz + mz * s + mw * zw + mxyz * xy + mxyw * xyzw - mxzw * xw - myzw * yw;
	float w = -mx * xw - my * yw - mz * zw + mw * s - mxyz * xyzw + mxyw * xy + mxzw * xz + myzw * yz;
	float xyz = -mx * yz + my * xz - mz * xy - mw * xyzw + mxyz * s + mxyw * zw - mxzw * yw + myzw * xw;
	float xyw = -mx * yw + my * xw + mz * xyzw - mw * xy - mxyz * zw + mxyw * s + mxzw * yz - myzw * xz;
	float xzw = -mx * zw - my * xyzw + mz * xw - mw * xz + mxyz * yw - mxyw * yz + mxzw * s + myzw * xy;
	float yzw = mx * xyzw - my * zw + mz * yw - mw * yz - mxyz * xw + mxyw * xz - mxzw * xy + myzw * s;

	glm::vec4 res(x, y, z, w);
	return res;
}

void Rotor::rotate(Rotor& a) {

	float as = a.getScalar();
	float axy = a.getXY();
	float axz = a.getXZ();
	float axw = a.getXW();
	float ayz = a.getYZ();
	float ayw = a.getYW();
	float azw = a.getZW();
	float axyzw = a.getXYZW();


	float tempS = as * s - axy * xy - axz * xz - axw * xw - ayw * yw - azw * zw + axyzw * xyzw;
	float tempXY = as * xy + axy * s - axz * yz - axw * yw + ayw * xw - azw * xyzw - axyzw * zw;
	float tempXZ = as * xz + axy * yz + axz * s - axw * zw - ayw * xyzw + azw * xw + axyzw * yw;
	float tempXW = as* xw + axy * yw + axz * zw + axw * s - ayw * xy - azw * xz - axyzw * yz;
	float tempYZ = as* yz - axy * xz + axz * xy - axw * xyzw - ayw * zw + azw * yw - axyzw * xw;
	float tempYW = as* yw - axy * xw + axz * xyzw + axw * xy + ayw * s - azw * yz + axyzw * xz;
	float tempZW = as* zw - axy * xyzw - axz * xw + axw * xz + ayw * yz + azw * s - axyzw * xy;
	float tempXYZW = as* xyzw + axy * zw - axz * yw + axw * yz - ayw * xz + azw * xy + axyzw * s;

	s = tempS;
	xy = tempXY;
	xz = tempXZ;
	xw = tempXW;
	yz = tempYZ;
	yw = tempYW;
	zw = tempZW;
	xyzw = tempXYZW;

}

glm::mat4 Rotor::toMatrix() {
	normalize();
	glm::vec4 x = apply(glm::vec4(1.f, 0.f, 0.f, 0.f));
	glm::vec4 y = apply(glm::vec4(0.f, 1.f, 0.f, 0.f));
	glm::vec4 z = apply(glm::vec4(0.f, 0.f, 1.f, 0.f));
	glm::vec4 w = apply(glm::vec4(0.f, 0.f, 0.f, 1.f));

	return glm::mat4(x, y, z, w);
}

void Rotor::reset() {
	s = 1.f;
	xy = 0.f;
	xz = 0.f;
	xw = 0.f;
	yz = 0.f;
	yw = 0.f;
	zw = 0.f;
	xyzw = 0.f;
}

