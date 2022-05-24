#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

static const float ZERO_TOLERANCE = 0.0000001f;
static const float _2PI = M_PI * 2;

static inline bool fequals_zero(const float& f) {
	return (fabs(f) < ZERO_TOLERANCE);
}

class Vector2 {
public:
	float x = 0.f;
	float y = 0.f;

	Vector2() = default;

	Vector2(float x, float y) : x{ x }, y{ y } {};

	Vector2& operator+(const Vector2& other) {
		Vector2 sum = Vector2(this->x + other.x, this->y + other.y);
		return sum;
	}
};

class VectorSpherical {
public:
	float theta  = 0.f;
	float phi = 0.f;

	VectorSpherical() = default;

	VectorSpherical(float theta, float phi) : theta{ theta }, phi{ phi } {};

	VectorSpherical& operator+(const VectorSpherical& other) {
		VectorSpherical sum = VectorSpherical(this->theta + other.theta, this->phi + other.phi);
		return sum;
	}
};


class Vector3 {
public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;

	Vector3() = default;

	Vector3(float x, float y, float z) : x{ x }, y{ y }, z{ z } {};

	Vector3& operator+(const Vector3& other) {
		Vector3 sum = Vector3(this->x + other.x, this->y + other.y, this->z + other.z);
		return sum;
	}
};