#pragma once

#pragma warning(push, 0)
#include "SDL.h"
#include "SDL_image.h"
#pragma warning(pop)
#undef main

#include "types.h"

int main();
void handleEvents();
void update();
void render();

void addStarsToConstellation(std::vector<std::pair<int, int>>& constellation, const int star_a, const int star_b);
bool screencoordsInBounds(Vector2 screen_coords, float Z);
inline Vector2 getScreenCoords(const float scalar, const Vector2& coords_n);
SDL_Texture* drawStars();
void ReadCSV(std::string filename, bool has_header = true);

static inline std::string& ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) { return !std::isspace(c); }));
	return s;
}

static inline std::string& rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
    return s;
}

// trim from both ends (in place)
static inline std::string& trim(std::string& s) {
    return ltrim(rtrim(s));
}