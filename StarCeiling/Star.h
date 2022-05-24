#pragma once

#include "types.h"
#include <string>
#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>

class Star
{

public:

	const float MAX_MAGNITUDE = -1.5f;
	const float MIN_MAGNITUDE = 7.f;
	const float DEFAULT_MAGNITUDE = MIN_MAGNITUDE;
	const float DEFAULT_B_V = 0.58;
	static const int MIN_TEMP = 1000; // kelvin
	static const int MAX_TEMP = 40000; // kelvin
	static const uint8_t MIN_BRIGHTNESS = 0;
	static const uint8_t MAX_BRIGHTNESS = 255;

	struct StarColour {
		uint8_t R = 0;
		uint8_t G = 0;
		uint8_t B = 0;
	};

	struct HSL {
		float H = 0;
		float S = 0;
		float L = 0;
	};

private:
	int id_ = 0;
	int hip_ = 0;
	int hd_ = 0;
	int hr_ = 0;
	std::string name_ = "";
	float magnitude_ = 0.f;
	uint8_t brightness_ = 0;
	float colour_index_ = 0.f;
	Vector3 location_absolute_ = Vector3(0.f, 0.f, 0.f); // before transforms, normalized
	Vector3 location_relative_ = Vector3(0.f, 0.f, 0.f); // after transform, normalized
	VectorSpherical spherical_ = VectorSpherical(0.f, 0.f); // theta, phi
	VectorSpherical spherical_n_ = VectorSpherical(0.f, 0.f); // theta, phi - normalized
	Vector2 screen_coords_ = Vector2(0.f, 0.f); // X, Y, normalized
	StarColour colour_ = { 0,0,0 };
	unsigned int temp_ = 0;

	void NormalizeAbsoluteLocation() {
		double r = sqrt(location_absolute_.x*location_absolute_.x + location_absolute_.y*location_absolute_.y + location_absolute_.z*location_absolute_.z);
		location_absolute_.x /= r;
		location_absolute_.y /= r;
		location_absolute_.z /= r;
	}

	HSL rgb_to_hsl(const StarColour rgb) {
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

	StarColour hsl_to_rgb(const HSL hsl) {
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

	void SetBrightness() {
		float brightness_n = (1.f - magnitude_ / (MIN_MAGNITUDE - MAX_MAGNITUDE));
		brightness_ = std::clamp(static_cast<uint8_t>(MIN_BRIGHTNESS + std::max(MAX_BRIGHTNESS - MIN_BRIGHTNESS, 0) * brightness_n), static_cast<uint8_t>(0), UINT8_MAX);
	}

public:

	Star() = default;

	inline void SetID(const int id) {
		id_ = id;
	}

	inline void SetID(const std::string id) {
		if (id.length() == 0) return;
		id_ = std::stoi(id);
	}

	inline int GetID() const {
		return id_;
	}

	inline void SetHIP(const int hip) {
		hip_ = hip;
	}

	inline void SetHIP(const std::string hip) {
		if (hip.length() == 0) return;
		hip_ = std::stoi(hip);
	}

	inline int GetHIP() const {
		return hip_;
	}

	inline void SetHD(const int hd) {
		hd_ = hd;
	}

	inline void SetHD(const std::string hd) {
		if (hd.length() == 0) return;
		hd_ = std::stoi(hd);
	}


	inline int GetHD() const {
		return hd_;
	}

	inline void SetHR(const int hr) {
		hr_ = hr;
	}

	inline void SetHR(const std::string hr) {
		if (hr.length() == 0) return;
		hr_ = std::stoi(hr);
	}

	inline int GetHR() const {
		return hr_;
	}

	inline void SetName(const std::string name) {
		name_ = name;
	}

	inline std::string GetName() const {
		return name_;
	}

	inline void SetMagnitude(const float magnitude) {
		magnitude_ = magnitude;
		SetBrightness();
	}

	inline void SetMagnitude(const std::string magnitude) {
		if (magnitude.length() == 0) {
			magnitude_ = DEFAULT_MAGNITUDE; // default
		}
		else {
			magnitude_ = std::stof(magnitude);
		}

		SetBrightness();
	}

	inline float GetMagnitude() const {
		return magnitude_;
	}

	inline void SetColourIndex(const float ci) {
		colour_index_ = ci;
		temp_ = ColourIndexToTemperature(colour_index_);
		colour_ = TemperatureToColour(temp_);
	}

	inline void SetColourIndex(const std::string ci) {
		if (ci.length() == 0) {
			// default
			colour_index_ = DEFAULT_B_V;
		}
		else {
			colour_index_ = std::stof(ci);
		}

		temp_ = ColourIndexToTemperature(colour_index_);
		colour_ = TemperatureToColour(temp_);
		HSL hsl = rgb_to_hsl(colour_);
		hsl.L = 100.f;
		hsl.S *= 0.4f;
		colour_ = hsl_to_rgb(hsl);
	}


	inline float GetColourIndex() const {
		return colour_index_;
	}

	inline StarColour GetColour() const {
		return colour_;
	}

	void CalculateSphericalCoords() {
		// theta
		spherical_.theta = acos(location_relative_.z);

		// phi
		spherical_.phi = atan2(location_relative_.y, location_relative_.x);

		// normalize
		spherical_n_.theta = 2 * spherical_.theta / M_PI;
		spherical_n_.phi = spherical_.phi / _2PI + 0.5f;
	}

	// calculate normalized screen coords from spherical coords
	void CalculateScreenCoords() {
		float r = spherical_n_.theta;
		screen_coords_ = Vector2(r * cos(spherical_.phi),
			r * sin(spherical_.phi));
	}

	void UpdateTransforms() {
		CalculateSphericalCoords();
		CalculateScreenCoords();
	}

	inline void SetLocation(const float x, const float y, const float z) {
		location_absolute_.x = x;
		location_absolute_.y = y;
		location_absolute_.z = z;
		NormalizeAbsoluteLocation();
		location_relative_ = location_absolute_;
		UpdateTransforms();
	}

	inline void SetLocation(const Vector3& pos) {
		SetLocation(pos.x, pos.y, pos.z);
	}

	// Gets this star's relative location
	inline Vector3 GetLocation() const {
		return location_relative_;
	}

	// Gets this star's absolute location before any transforms
	inline Vector3 GetWorldLocation() const {
		return location_absolute_;
	}

	inline VectorSpherical& GetSphericalCoords() {
		return spherical_;
	}

	// get normalized spherical coords
	inline VectorSpherical& GetSphericalCoordsN() {
		return spherical_n_;
	}

	// Gets this star's relative X coordinate
	inline float GetX() const {
		return location_relative_.x;
	}

	// Gets this star's relative Y coordinate
	inline float GetY() const {
		return location_relative_.y;
	}

	// Gets this star's relative Z coordinate
	inline float GetZ() const {
		return location_relative_.z;
	}

	void Rotate_X(float angle) {
		Rotate_X(location_relative_, angle);
	}

	void Rotate_X(const Vector3& v, float angle) {
		float cosa = cos(angle);
		float sina = sin(angle);
		Vector3 new_loc = {};
		new_loc.x = v.x;
		new_loc.y = v.y * cosa - v.z * sina;
		new_loc.z = v.y * sina + v.z * cosa;
		location_relative_ = new_loc;
	}

	void Rotate_Y(float angle) {
		Rotate_Y(location_relative_, angle);
	}

	void Rotate_Y(const Vector3& v, float angle) {
		float cosa = cos(angle);
		float sina = sin(angle);
		Vector3 new_loc = {};
		new_loc.x = v.x * cosa + v.z * sina;
		new_loc.y = v.y;
		new_loc.z = v.z * cosa - v.x * sina;
		location_relative_ = new_loc;
	}

	void Rotate_Z(float angle) {
		Rotate_Z(location_relative_, angle);
	}

	void Rotate_Z(const Vector3& v, float angle) {
		float cosa = cos(angle);
		float sina = sin(angle);
		Vector3 new_loc = {};
		new_loc.x = v.x * cosa - v.y * sina;
		new_loc.y = v.x * sina + v.y * cosa;
		new_loc.z = v.z;
		location_relative_ = new_loc;
	}

	inline Vector2 GetScreenCoords() const {
		return screen_coords_;
	}

	inline uint8_t GetBrightness() const {
		return brightness_;
	}

	/*
		Convert colour temperature to RGB
		input: colour temperature in kelvin
		output: R, G and B channels representing this colour
		Red values below 6600 K clamped to 255
		Blue values below 2000 K are always 0
		Blue values above 6500 K are always 255
	*/

	static unsigned int ColourIndexToTemperature(const float B_V) {
		return (5601 / pow(B_V + 0.4, 2.f / 3.f));
	}

	static StarColour TemperatureToColour(unsigned int temp) {
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
};

