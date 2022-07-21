#pragma once

#include <string>

#include "types.h"

class Star
{

public:

	static const long MAX_MAGNITUDE = static_cast<long>(-1.5);
	static const long MIN_MAGNITUDE = static_cast<long>(8.0);
	static const long DEFAULT_MAGNITUDE = MIN_MAGNITUDE;
	static const long DEFAULT_B_V = static_cast<long>(0.58);
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


	unsigned int ColourIndexToTemperature(const float B_V) {
		return static_cast<unsigned int>(5601 / pow(B_V + 0.4, 2.f / 3.f));
	}

	void CalculateSphericalCoords();
	void NormalizeAbsoluteLocation();

public:

	Star();

	void SetAbsoluteLocation(const float x, const float y, const float z);

	void SetAbsoluteLocation(const Vector3<float>& pos) {
		SetAbsoluteLocation(pos.x, pos.y, pos.z);
	}

	// Gets this star's relative location
	Vector3<float> GetLocation() const {
		return location_relative_;
	}

	// Gets this star's absolute location before any transforms
	Vector3<float> GetAbsoluteLocation() const {
		return location_absolute_;
	}

	VectorSpherical& GetSphericalCoords() {
		return spherical_;
	}

	// get normalized spherical coords
	VectorSpherical& GetSphericalCoordsN() {
		return spherical_n_;
	}

	// Gets this star's relative X coordinate
	float GetX() const {
		return location_relative_.x;
	}

	// Gets this star's relative Y coordinate
	float GetY() const {
		return location_relative_.y;
	}

	// Gets this star's relative Z coordinate
	float GetZ() const {
		return location_relative_.z;
	}

	float GetColourIndex() const {
		return colour_index_;
	}

	RGB GetColour() const {
		return colour_;
	}

	void SetBrightness();
	void SetColourIndex(const std::string ci);
	void SetColourIndex(const float ci);
	void Rotate_X(float angle);
	void Rotate_X(const Vector3<float>& v, float angle);
	void Rotate_Y(float angle);
	void Rotate_Y(const Vector3<float>& v, float angle);
	void Rotate_Z(float angle);
	void Rotate_Z(const Vector3<float>& v, float angle);

	void SetID(const std::string id);
	void SetID(const int id) {
		id_ = id;
	}
	int GetID() const {
		return id_;
	}

	void SetHIP(const std::string hip);
	void SetHIP(const int hip) {
		hip_ = hip;
	}
	int GetHIP() const {
		return hip_;
	}

	void SetHD(const std::string hd);
	void SetHD(const int hd) {
		hd_ = hd;
	}
	int GetHD() const {
		return hd_;
	}

	void SetHR(const std::string hr);
	void SetHR(const int hr) {
		hr_ = hr;
	}
	int GetHR() const {
		return hr_;
	}

	void SetName(const std::string name) {
		name_ = name;
	}

	std::string GetName() const {
		return name_;
	}

	void SetMagnitude(const float magnitude);
	void SetMagnitude(const std::string magnitude);

	float GetMagnitude() const {
		return magnitude_;
	}

	uint8_t GetBrightness() const {
		return brightness_;
	}

	// calculate normalized screen coords from spherical coords
	void CalculateScreenCoords() {
		screen_coords_ = Vector2<float>(spherical_n_.theta * static_cast<float>(cos(spherical_.phi)), spherical_n_.theta * static_cast<float>(sin(spherical_.phi)));
	}

	Vector2<float> GetScreenCoords() const {
		return screen_coords_;
	}

	void UpdateTransforms();

	static RGB TemperatureToColour(unsigned int temp);
};

