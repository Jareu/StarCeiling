#pragma once

#pragma warning(push, 0)
#include "SDL.h"
#include "SDL_image.h"
#pragma warning(pop)
#undef main

int main();
void handleEvents();
void update();
void render();

SDL_Texture* draw_stars();
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