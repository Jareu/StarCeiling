#pragma once

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <math.h>

template <typename T>
class Vector2 {
public:
	T x = 0;
	T y = 0;

	Vector2() = default;

	Vector2(T x, T y) : x{ x }, y{ y } {};

	Vector2 operator+(const Vector2<typename T>& rhs) {
		Vector2 sum = Vector2(this->x + rhs.x, this->y + rhs.y);
		return sum;
	}

	Vector2 operator-(const Vector2<typename T>& rhs) {
		Vector2 subtraction = Vector2(this->x - rhs.x, this->y - rhs.y);
		return subtraction;
	}

	Vector2& operator+=(const Vector2<typename T>& rhs) {
		this->x = this->x + rhs.x;
		this->y = this->y + rhs.y;
		return *this;
	}

	Vector2& operator-=(const Vector2<typename T>& rhs) {
		this->x = this->x - rhs.x;
		this->y = this->y - rhs.y;
		return *this;
	}
};

template <typename T>
class Vector3 {
public:
	T x = 0;
	T y = 0;
	T z = 0;

	Vector3() = default;

	Vector3(T x, T y, T z) : x{ x }, y{ y }, z{ z } {};

	Vector3 operator+(const Vector3<typename T>& rhs) {
		Vector3 sum = Vector3(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z);
		return sum;
	}

	Vector3 operator-(const Vector3<typename T>& rhs) {
		Vector3 subtraction = Vector3(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z);
		return subtraction;
	}

	Vector3& operator+=(const Vector3<typename T>& rhs) {
		this->x = this->x + rhs.x;
		this->y = this->y + rhs.y;
		this->z = this->z + rhs.z;
		return *this;
	}

	Vector3& operator-=(const Vector3<typename T>& rhs) {
		this->x = this->x - rhs.x;
		this->y = this->y - rhs.y;
		this->z = this->z - rhs.z;
		return *this;
	}
};

class VectorSpherical {
public:
	float theta = 0.f;
	float phi = 0.f;

	VectorSpherical() = default;

	VectorSpherical(float theta, float phi) : theta{ theta }, phi{ phi } {};

	VectorSpherical operator+(const VectorSpherical& other) {
		VectorSpherical sum = VectorSpherical(this->theta + other.theta, this->phi + other.phi);
		return sum;
	}

	VectorSpherical operator-(const VectorSpherical& other) {
		VectorSpherical sum = VectorSpherical(this->theta - other.theta, this->phi - other.phi);
		return sum;
	}
};


template <typename T>
class Range {
public:
	T min = 0;
	T max = 0;

	Range() = default;
	Range(T min, T max) : min{ min }, max{ max } {};
};

struct RGB {
	uint8_t R = 0;
	uint8_t G = 0;
	uint8_t B = 0;
};

struct RGBA {
	uint8_t R = 0;
	uint8_t G = 0;
	uint8_t B = 0;
	uint8_t A = 0;
};

struct HSL {
	float H = 0;
	float S = 0;
	float L = 0;
};

// Fonts
enum class eFontSize {
	SMALL,
	MEDIUM,
	LARGE,
	TITLE
};

// stars
enum class StarSize {
	NONE,
	SMALL,
	MEDIUM,
	LARGE
};