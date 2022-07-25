
#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <chrono>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <map>
#include <utility>

#include "utilities.h"
#include "globals.h"


bool screencoordsInBounds(Vector2<int> screen_coords, float Z) {
	bool x_in_bounds = screen_coords.x > 0 && screen_coords.x < ceiling_size.x;
	bool y_in_bounds = screen_coords.y > 0 && screen_coords.y < ceiling_size.y;
	//bool z_in_bounds = Z > 0.f;
	bool z_in_bounds = true;
	return (x_in_bounds && y_in_bounds && z_in_bounds);
}

RGB hsl_to_rgb(const HSL hsl) {
	RGB rgb = { 0, 0, 0 };

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

HSL rgb_to_hsl(const RGB rgb) {
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

void updateZoom() {
	zoom = exp(log_min_zoom + (log_max_zoom - log_min_zoom) * zoom_steps / (MAX_ZOOM - 1));
	bStarsChanged = true;
}

Vector2<int> getScreenCoords(const float scalar, const Vector2<float>& coords_n) {
	// TODO: update below to a global variable updated only once
	int x_half = ceiling_size.x / 2;
	int y_half = ceiling_size.y / 2;
	return Vector2<int>(static_cast<int>(round(scalar * coords_n.x + x_half)), static_cast<int>(round(scalar * coords_n.y + y_half)));
}

bool fequals_zero(const float& f) {
	return (fabs(f) < ZERO_TOLERANCE);
}

void increment_time(const float delta_seconds) {
	earth_rotation = fmod(earth_rotation + _2PI * delta_seconds / SECONDS_IN_A_DAY, _2PI);
}

void resetStarCount() {
	num_stars_large = 0;
	num_stars_medium = 0;
	num_stars_small = 0;
}

void correctStarRotation(const double& angle) {
	// rotate stars by 90 degrees
	auto universe_i = universe.begin();
	while (universe_i != universe.end())
	{
		// get value from star
		auto& star = universe_i->second;

		if (star) {
			// rotate around Y axis by time of day, then rotate about X axis by latitude
			Vector3 new_loc = star->GetAbsoluteLocation();
			star->Rotate_X(static_cast<float>(angle));
			star->SetAbsoluteLocation(star->GetLocation());
		}

		universe_i++;
	}
}

/*
	Sorts small, medium, and large groups of stars each by magnitude.
*/
void sortStars() {
	std::sort(small_stars.begin(), small_stars.end(), sortStarsByMagnitude);
	std::sort(medium_stars.begin(), medium_stars.end(), sortStarsByMagnitude);
	std::sort(large_stars.begin(), large_stars.end(), sortStarsByMagnitude);
}

/*
	Applies star magnitude thresholds to split stars up into groups of small, medium, and large
*/
void groupStarBySize(const Star& star) {
	// get star's ID and magnitude
	auto id_magnitude = std::pair<int, float>{ star.GetID(), star.GetMagnitude()};

	// figure out where to put it
	if (star.GetMagnitude() >= star_threshold_large.min && star.GetMagnitude() < star_threshold_large.max) {
		// Large
		large_stars.push_back(id_magnitude);
	}
	else if (star.GetMagnitude() >= star_threshold_medium.min && star.GetMagnitude() < star_threshold_medium.max) {
		// Medium
		medium_stars.push_back(id_magnitude);
	}
	else if (star.GetMagnitude() >= star_threshold_small.min && star.GetMagnitude() < star_threshold_small.max) {
		// Small
		small_stars.push_back(id_magnitude);
	}
}