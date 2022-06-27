#include "Star.h"

void Star::CalculateSphericalCoords() {
	// theta
	spherical_.theta = static_cast<float>(acos(location_relative_.z));

	// phi
	spherical_.phi = static_cast<float>(atan2(location_relative_.y, location_relative_.x));

	// normalize
	spherical_n_.theta = 2.f * spherical_.theta / static_cast<float>(M_PI);
	spherical_n_.phi = spherical_.phi / _2PI + 0.5f;
}

void Star::Rotate_X(float angle) {
	Rotate_X(location_relative_, angle);
}

void Star::Rotate_X(const Vector3<float>& v, float angle) {
	float cosa = static_cast<float>(cos(angle));
	float sina = static_cast<float>(sin(angle));
	Vector3<float> new_loc = {};
	new_loc.x = v.x;
	new_loc.y = v.y * cosa - v.z * sina;
	new_loc.z = v.y * sina + v.z * cosa;
	location_relative_ = new_loc;
}

void Star::Rotate_Y(float angle) {
	Rotate_Y(location_relative_, angle);
}

void Star::Rotate_Y(const Vector3<float>& v, float angle) {
	float cosa = static_cast<float>(cos(angle));
	float sina = static_cast<float>(sin(angle));
	Vector3<float> new_loc = {};
	new_loc.x = v.x * cosa + v.z * sina;
	new_loc.y = v.y;
	new_loc.z = v.z * cosa - v.x * sina;
	location_relative_ = new_loc;
}

void Star::Rotate_Z(float angle) {
	Rotate_Z(location_relative_, angle);
}

void Star::Rotate_Z(const Vector3<float>& v, float angle) {
	float cosa = static_cast<float>(cos(angle));
	float sina = static_cast<float>(sin(angle));
	Vector3<float> new_loc = {};
	new_loc.x = v.x * cosa - v.y * sina;
	new_loc.y = v.x * sina + v.y * cosa;
	new_loc.z = v.z;
	location_relative_ = new_loc;
}

/*
	TemperatureToColour()
	Convert colour temperature to RGB
	input: colour temperature in kelvin
	output: R, G and B channels representing this colour
	Red values below 6600 K clamped to 255
	Blue values below 2000 K are always 0
	Blue values above 6500 K are always 255
*/

RGB Star::TemperatureToColour(unsigned int temp) {
	// default to white
	RGB output = RGB{ UINT8_MAX, UINT8_MAX, UINT8_MAX };

	temp = std::clamp(temp, static_cast<unsigned int>(MIN_TEMP), static_cast<unsigned int>(MAX_TEMP));

	// Calculate Red
	if (temp <= 6600) {
		output.R = UINT8_MAX;
	}
	else {
		output.R = std::clamp(static_cast<uint8_t> (329.698727446 / pow(temp - 6000, 0.1332047592)), static_cast<uint8_t>(0), UINT8_MAX);
	}

	// Calculate Green
	if (temp <= 6600) {
		output.G = std::clamp(static_cast<uint8_t> (99.4708025861 * log(temp) - 161.1195681661), static_cast<uint8_t>(0), UINT8_MAX);
	}
	else {
		output.G = std::clamp(static_cast<uint8_t> (288.1221695283 / pow(temp - 6000, 0.0755148492)), static_cast<uint8_t>(0), UINT8_MAX);
	}

	// Calculate Blue
	if (temp >= 6600) {
		output.B = UINT8_MAX;
	}
	else if (temp <= 1900) {
		output.B = std::clamp(static_cast<uint8_t> (288.1221695283 / pow(temp - 6000, 0.0755148492)), static_cast<uint8_t>(0), UINT8_MAX);
	}
	else {
		output.B = std::clamp(static_cast<uint8_t> (138.5177312231 * log(temp - 1000) - 305.0447927307), static_cast<uint8_t>(0), UINT8_MAX);
	}

	return output;
}