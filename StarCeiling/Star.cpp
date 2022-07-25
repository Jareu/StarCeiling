
#include <algorithm>

#include "Star.h"
#include "globals.h"
#include "utilities.h"

Star::Star() {

}

void Star::NormalizeAbsoluteLocation() {
	float r = static_cast<float>(sqrt(location_absolute_.x * location_absolute_.x + location_absolute_.y * location_absolute_.y + location_absolute_.z * location_absolute_.z));
	location_absolute_.x /= r;
	location_absolute_.y /= r;
	location_absolute_.z /= r;
}

void Star::CalculateSphericalCoords() {
	// theta
	spherical_.theta = static_cast<float>(acos(location_relative_.z));

	// phi
	spherical_.phi = static_cast<float>(atan2(location_relative_.y, location_relative_.x));

	// normalize
	spherical_n_.theta = 2.f * spherical_.theta / static_cast<float>(M_PI);
	spherical_n_.phi = spherical_.phi / _2PI + 0.5f;
}

void Star::SetAbsoluteLocation(const float x, const float y, const float z) {
	location_absolute_.x = x;
	location_absolute_.y = y;
	location_absolute_.z = z;
	NormalizeAbsoluteLocation();
	location_relative_ = location_absolute_;
	UpdateTransforms();
}

void Star::Rotate_X(double angle) {
	Rotate_X(location_relative_, angle);
}

void Star::Rotate_X(const Vector3<float>& v, double angle) {
	double cosa = cos(angle);
	double sina = sin(angle);
	Vector3<float> new_loc = {};
	new_loc.x = v.x;
	new_loc.y = static_cast<float>(v.y * cosa - v.z * sina);
	new_loc.z = static_cast<float>(v.y * sina + v.z * cosa);
	location_relative_ = new_loc;
}

void Star::Rotate_Y(double angle) {
	Rotate_Y(location_relative_, angle);
}

void Star::Rotate_Y(const Vector3<float>& v, double angle) {
	double cosa = cos(angle);
	double sina = sin(angle);
	Vector3<float> new_loc = {};
	new_loc.x = static_cast<float>(v.x * cosa + v.z * sina);
	new_loc.y = v.y;
	new_loc.z = static_cast<float>(v.z * cosa - v.x * sina);
	location_relative_ = new_loc;
}

void Star::Rotate_Z(double angle) {
	Rotate_Z(location_relative_, angle);
}

void Star::Rotate_Z(const Vector3<float>& v, double angle) {
	double cosa = cos(angle);
	double sina = sin(angle);
	Vector3<float> new_loc = {};
	new_loc.x = static_cast<float>(v.x * cosa - v.y * sina);
	new_loc.y = static_cast<float>(v.x * sina + v.y * cosa);
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

void Star::SetColourIndex(const std::string ci)
{
	if (ci.length() == 0) {
		// default
		colour_index_ = DEFAULT_B_V;
	}
	else {
		colour_index_ = std::stof(ci);
	}

	temp_ = ColourIndexToTemperature(colour_index_);
	colour_ = TemperatureToColour(temp_);
	HSL hsl = rgb_to_hsl(static_cast<const RGB>(colour_));
	hsl.L = 100.f;
	hsl.S *= 0.4f;
	colour_ = hsl_to_rgb(hsl);
}

void Star::SetHIP(const std::string hip) {
	if (hip.length() == 0) return;
	hip_ = std::stoi(hip);
}

void Star::SetID(const std::string id) {
	if (id.length() == 0) return;
	id_ = std::stoi(id);
}

void Star::SetHD(const std::string hd) {
	if (hd.length() == 0) return;
	hd_ = std::stoi(hd);
}

void Star::SetHR(const std::string hr) {
	if (hr.length() == 0) return;
	hr_ = std::stoi(hr);
}

void Star::SetMagnitude(const float magnitude) {
	magnitude_ = magnitude;
	SetBrightness();
}

void Star::SetMagnitude(const std::string magnitude) {
	if (magnitude.length() == 0) {
		magnitude_ = DEFAULT_MAGNITUDE; // default
	}
	else {
		magnitude_ = std::stof(magnitude);
	}

	SetBrightness();
}

void Star::SetColourIndex(const float ci) {
	colour_index_ = ci;
	temp_ = ColourIndexToTemperature(ci);
	colour_ = TemperatureToColour(temp_);
}

void Star::UpdateTransforms() {
	CalculateSphericalCoords();
	CalculateScreenCoords();
}

void Star::SetBrightness() {
	float brightness_n = (1.f - magnitude_ / (MIN_MAGNITUDE - MAX_MAGNITUDE));
	brightness_ = std::clamp(static_cast<uint8_t>(MIN_BRIGHTNESS + std::max(MAX_BRIGHTNESS - MIN_BRIGHTNESS, 0) * brightness_n), static_cast<uint8_t>(0), UINT8_MAX);
}