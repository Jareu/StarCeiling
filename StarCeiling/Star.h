#pragma once

#include <string>
#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>

#include "types.h"
#include "graphics.h"

class Star
{

public:

	static const long MAX_MAGNITUDE = -1.5;
	static const long MIN_MAGNITUDE = 8.0;
	static const long DEFAULT_MAGNITUDE = MIN_MAGNITUDE;
	static const long DEFAULT_B_V = 0.58;
	static const int MIN_TEMP = 1000; // kelvin
	static const int MAX_TEMP = 40000; // kelvin
	static const uint8_t MIN_BRIGHTNESS = 0;
	static const uint8_t MAX_BRIGHTNESS = 255;

private:
	int id_ = 0;
	int hip_ = 0;
	int hd_ = 0;
	int hr_ = 0;
	std::string name_ = "";
	float magnitude_ = 0.f;
	uint8_t brightness_ = 0;
	float colour_index_ = 0.f;
	Vector3<float> location_absolute_ = Vector3<float>(0.f, 0.f, 0.f); // before transforms, normalized
	Vector3<float> location_relative_ = Vector3<float>(0.f, 0.f, 0.f); // after transform, normalized
	VectorSpherical spherical_ = VectorSpherical(0.f, 0.f); // theta, phi
	VectorSpherical spherical_n_ = VectorSpherical(0.f, 0.f); // theta, phi - normalized
	Vector2<float> screen_coords_ = Vector2<float>(0.f, 0.f); // X, Y, normalized
	RGB colour_ = { 0,0,0 };
	unsigned int temp_ = 0;

	inline void NormalizeAbsoluteLocation() {
		float r = static_cast<float>(sqrt(location_absolute_.x*location_absolute_.x + location_absolute_.y*location_absolute_.y + location_absolute_.z*location_absolute_.z));
		location_absolute_.x /= r;
		location_absolute_.y /= r;
		location_absolute_.z /= r;
	}

	inline void SetBrightness() {
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

	inline RGB GetColour() const {
		return colour_;
	}

	void CalculateSphericalCoords();

	// calculate normalized screen coords from spherical coords
	inline void CalculateScreenCoords() {
		float r = spherical_n_.theta;
		screen_coords_ = Vector2<float>(r * static_cast<float>(cos(spherical_.phi)), r * static_cast<float>(sin(spherical_.phi)));
	}

	inline void UpdateTransforms() {
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

	inline void SetLocation(const Vector3<float>& pos) {
		SetLocation(pos.x, pos.y, pos.z);
	}

	// Gets this star's relative location
	inline Vector3<float> GetLocation() const {
		return location_relative_;
	}

	// Gets this star's absolute location before any transforms
	inline Vector3<float> GetWorldLocation() const {
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

	void Rotate_X(float angle);
	void Rotate_X(const Vector3<float>& v, float angle);

	void Rotate_Y(float angle);
	void Rotate_Y(const Vector3<float>& v, float angle);

	void Rotate_Z(float angle);
	void Rotate_Z(const Vector3<float>& v, float angle);

	inline Vector2<float> GetScreenCoords() const {
		return screen_coords_;
	}

	inline uint8_t GetBrightness() const {
		return brightness_;
	}

	static inline unsigned int ColourIndexToTemperature(const float B_V) {
		return static_cast<unsigned int>(5601 / pow(B_V + 0.4, 2.f / 3.f));
	}

	static RGB TemperatureToColour(unsigned int temp);
};