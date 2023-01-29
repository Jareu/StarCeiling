#pragma once

#include <string>
#include <vector>
#include "Star.h"
#include "types.h"

static void addStarsToConstellation(std::vector<std::pair<int, int>>& constellation, const int star_a, const int star_b) {
	constellation.push_back(std::pair<int, int>(star_a, star_b));
}

inline std::string& ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) { return !std::isspace(c); }));
	return s;
}

inline std::string& rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
	return s;
}

// trim from both ends (in place)
inline std::string& trim(std::string& s) {
	return ltrim(rtrim(s));
}

/* Takes a std vector of strings, returns the value at the given index.
Out of bounds exception returns an empty string.
*/
inline std::string getValueFromIndex(std::vector<std::string>& v, const unsigned int index) {
	return (index >= v.size()) ? "" : trim(v[index]);
}

bool screencoordsInBounds(Vector2<int> screen_coords, float Z);
RGB hsl_to_rgb(const HSL hsl);
HSL rgb_to_hsl(const RGB rgb);
void updateZoom();
Vector2<int> getScreenCoords(const float scalar, const Vector2<float>& coords_n);
bool fequals_zero(const float& f);
void increment_time(const float delta_seconds);
void resetStarCount();
inline bool sortStarsByMagnitude(const std::pair<int, float>& a, const std::pair<int, float>& b) { return (a.second < b.second); }
void correctStarRotation(const double& angle);
void updateScreenProperties();
void updateSegment(int id, Vector2<float> screen_coords, StarSize size);
void clearSegments();