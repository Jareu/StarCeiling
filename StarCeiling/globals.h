#pragma once

#pragma warning(push, 0)
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#pragma warning(pop)
#undef main

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "Star.h"
#include "types.h"

namespace Environment {
	inline SDL_Renderer* renderer = NULL;
	inline SDL_Window* window = NULL;
	inline TTF_Font* font_small = NULL;
	inline TTF_Font* font_medium = NULL;
	inline TTF_Font* font_large = NULL;
	inline TTF_Font* font_title = NULL;
	inline bool bFontLoaded = false;
	static const std::string fontname = "arial";
	static bool bUseBlendedFonts = true;
}

inline bool bIsRunning = false;
inline bool bIsActive = false;
inline bool bStarsChanged = false;
inline bool bFullscreen = false;
inline bool bLoadingStars = true;

static const int WINDOW_WIDTH = 1920;
static const int WINDOW_HEIGHT = 1080;

static const int WINDOW_WIDTH_HALF = static_cast<int> (WINDOW_WIDTH / 2);
static const int WINDOW_HEIGHT_HALF = static_cast<int> (WINDOW_HEIGHT / 2);

static const uint8_t FONT_SIZE_SMALL = 16;
static const uint8_t FONT_SIZE_MEDIUM = 22;
static const uint8_t FONT_SIZE_LARGE = 28;
static const uint8_t FONT_SIZE_TITLE = 52;

static const float SECONDS_IN_A_DAY = 86400.f; // (86400 seconds = 1 day)
inline float earth_rotation = 0.f; // rotation of earth in radians
inline float latitude = 0.f; // latitude in radians, negative is south. 
static const float EARTH_ROTATION_RATE = 20.f;
static const RGB constellation_colour = RGB{ 255, 255, 255 };

// UI variables

// -- pan
inline Vector2<int> window_offset = { 0, 0 };		// offset from center
inline Vector2<int> window_pan_offset = { 0, 0 };	// offset from center prior to panning
inline bool mouse_btn_left = false;			// mouse button state
inline Vector2<int> cursor_pos = { 0, 0 };		// current cursor position
inline Vector2<int> cursor_pan_pos = { 0, 0 };	// cursor position when panning started

// -- Zoom
static const double window_scale = 0.45;
static const unsigned short MIN_ZOOM = 1;
static const unsigned short MAX_ZOOM = 50;
static const float log_min_zoom = static_cast<float>(log(MIN_ZOOM));
static const float log_max_zoom = static_cast<float>(log(MAX_ZOOM));
inline float zoom = 1.f;
inline unsigned short zoom_steps = MIN_ZOOM;

// Display area: should form a 6x5 grid
static const unsigned int margin = 50;
inline int ceiling_x = 6; // width in half metres
inline int ceiling_y = 5; // length in half metres
inline Vector2<int> ceiling_size = {};
inline Vector2<int> ceiling_offset = {};
inline float ceiling_aspect = ceiling_x / static_cast<float>(ceiling_y);
inline int segment_size = 0;
inline std::vector<std::vector<int>> segments = {};


static const float ZERO_TOLERANCE = 0.0000001f;
static const float _2PI = static_cast<float>(M_PI) * 2.f;

inline std::map<int, std::unique_ptr<Star>> sky = {};
inline std::vector<std::vector<std::pair<int, int>>> constellations = {};

inline SDL_Texture* star_texture = NULL;
inline SDL_Texture* ui_texture = NULL;
inline SDL_Rect star_rect = SDL_Rect{ 0,0,0,0 };
