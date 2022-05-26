#include "Star.h"


void Star::CalculateSphericalCoords() {
	// theta
	spherical_.theta = acos(location_relative_.z);

	// phi
	spherical_.phi = atan2(location_relative_.y, location_relative_.x);

	// normalize
	spherical_n_.theta = 2 * spherical_.theta / M_PI;
	spherical_n_.phi = spherical_.phi / _2PI + 0.5f;
}

void Star::Rotate_X(float angle) {
	Rotate_X(location_relative_, angle);
}

void Star::Rotate_X(const Vector3& v, float angle) {
	float cosa = cos(angle);
	float sina = sin(angle);
	Vector3 new_loc = {};
	new_loc.x = v.x;
	new_loc.y = v.y * cosa - v.z * sina;
	new_loc.z = v.y * sina + v.z * cosa;
	location_relative_ = new_loc;
}

void Star::Rotate_Y(float angle) {
	Rotate_Y(location_relative_, angle);
}

void Star::Rotate_Y(const Vector3& v, float angle) {
	float cosa = cos(angle);
	float sina = sin(angle);
	Vector3 new_loc = {};
	new_loc.x = v.x * cosa + v.z * sina;
	new_loc.y = v.y;
	new_loc.z = v.z * cosa - v.x * sina;
	location_relative_ = new_loc;
}

void Star::Rotate_Z(float angle) {
	Rotate_Z(location_relative_, angle);
}

void Star::Rotate_Z(const Vector3& v, float angle) {
	float cosa = cos(angle);
	float sina = sin(angle);
	Vector3 new_loc = {};
	new_loc.x = v.x * cosa - v.y * sina;
	new_loc.y = v.x * sina + v.y * cosa;
	new_loc.z = v.z;
	location_relative_ = new_loc;
}

Star::StarColour Star::hsl_to_rgb(const HSL hsl) {
	StarColour rgb = { 0, 0, 0 };

	if (hsl.H > 360.f || hsl.H < 0.f || hsl.S>100.f || hsl.S < 0.f || hsl.L>100.f || hsl.L < 0.f) {
		return rgb;
	}

	float s = hsl.S / 100.f;
	float v = hsl.L / 100.f;
	float C = s * v;
	float X = C * (1.f - static_cast<float>(abs(fmod(hsl.H / 60.f, 2) - 1.f)));
	float m = v - C;
	float r, g, b;

	if (hsl.H >= 0.f && hsl.H < 60.f) {
		r = C, g = X, b = 0.f;
	}
	else if (hsl.H >= 60.f && hsl.H < 120.f) {
		r = X, g = C, b = 0.f;
	}
	else if (hsl.H >= 120.f && hsl.H < 180.f) {
		r = 0.f, g = C, b = X;
	}
	else if (hsl.H >= 180.f && hsl.H < 240.f) {
		r = 0.f, g = X, b = C;
	}
	else if (hsl.H >= 240.f && hsl.H < 300.f) {
		r = X, g = 0.f, b = C;
	}
	else {
		r = C, g = 0.f, b = X;
	}

	unsigned char R = static_cast<unsigned char>(roundf((r + m) * 255.f));
	unsigned char G = static_cast<unsigned char>(roundf((g + m) * 255.f));
	unsigned char B = static_cast<unsigned char>(roundf((b + m) * 255.f));

	rgb = { R, G, B };
	return rgb;
}

HSL Star::rgb_to_hsl(const StarColour rgb) {
	HSL hsl = { 0.f, 0.f, 0.f };

	float r = rgb.R / 255.f;
	float g = rgb.G / 255.f;
	float b = rgb.B / 255.f;

	float rgb_max = std::max({ r, g, b });
	float rgb_min = std::min({ r, g, b });

	hsl.H = 0.f;
	hsl.L = 50.f * (rgb_min + rgb_max);
	hsl.S = 0.f;

	if (rgb_min == rgb_max) {
		hsl.S = 0.f;
		hsl.H = 0.f;
		return hsl;
	}
	else if (hsl.L < 50.f) {
		hsl.S = 100.f * (rgb_max - rgb_min) / (rgb_max + rgb_min);
	}
	else {
		hsl.S = 100.f * (rgb_max - rgb_min) / (2.f - rgb_max - rgb_min);
	}

	if (rgb_max == r) {
		hsl.H = 60.f * (g - b) / (rgb_max - rgb_min);
	}
	else if (rgb_max == g) {
		hsl.H = 60.f * (b - r) / (rgb_max - rgb_min) + 120.f;
	}
	else if (rgb_max == b) {
		hsl.H = 60.f * (r - g) / (rgb_max - rgb_min) + 240.f;
	}

	if (hsl.H < 0.f)
	{
		hsl.H = hsl.H + 360.f;
	}

	return hsl;
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

Star::StarColour Star::TemperatureToColour(unsigned int temp) {
	// default to white
	StarColour output = StarColour{ UINT8_MAX, UINT8_MAX, UINT8_MAX };

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