#pragma once

#pragma warning(push, 0)
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#pragma warning(pop)
#undef main

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>

#include "Star.h"
#include "types.h"
#include "Segment.h"

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

inline int WINDOW_WIDTH = 1920;
inline int WINDOW_HEIGHT = 1080;

inline int WINDOW_WIDTH_HALF = static_cast<int> (WINDOW_WIDTH / 2);
inline int WINDOW_HEIGHT_HALF = static_cast<int> (WINDOW_HEIGHT / 2);

static const uint8_t FONT_SIZE_SMALL = 16;
static const uint8_t FONT_SIZE_MEDIUM = 22;
static const uint8_t FONT_SIZE_LARGE = 28;
static const uint8_t FONT_SIZE_TITLE = 52;

static const float SECONDS_IN_A_DAY = 86400.f; // (86400 seconds = 1 day)
inline float earth_rotation = 0.f; // rotation of earth in radians
inline float latitude = 0.f; // latitude in radians, negative is south. 
static const float EARTH_ROTATION_RATE = 20.f;
static const bool bRotateStars = false;
static const RGB constellation_colour = RGB{ 255, 255, 255 };

static const int max_stars_small = 315;
static const int max_stars_medium = 270;
static const int max_stars_large = 48;

inline int num_stars_small = 0;
inline int num_stars_medium = 0;
inline int num_stars_large = 0;

inline std::vector <std::pair<int, float>> stars_by_magnitude = {};

static const float star_radius_medium = 0.75f;
static const float star_radius_large = 1.25f;

// UI variables

// -- pan
inline Vector2<int> window_offset = { 0, 0 };		// offset from center
inline Vector2<int> window_pan_offset = { 0, 0 };	// offset from center prior to panning
inline bool mouse_btn_left = false;			// mouse button state
inline Vector2<int> cursor_pos = { 0, 0 };		// current cursor position
inline Vector2<int> cursor_pan_pos = { 0, 0 };	// cursor position when panning started
inline bool bIsCursorInSky = false;

// -- Zoom
static const double window_scale = 0.45;
static const unsigned short MIN_ZOOM = 1;
static const unsigned short MAX_ZOOM = 50;
static const float log_min_zoom = static_cast<float>(log(MIN_ZOOM));
static const float log_max_zoom = static_cast<float>(log(MAX_ZOOM));
inline float zoom = 1.f;
inline float screen_coefficient = 1.f;
inline unsigned short zoom_steps = MIN_ZOOM;

// Display area: should form a 6x5 grid
static const unsigned int margin = 50;
inline int ceiling_x = 6; // width in half metres
inline int ceiling_y = 5; // length in half metres
inline Vector2<int> ceiling_size = { 0, 0 };
inline Vector2<int> ceiling_offset = { 0, 0, };
inline float ceiling_aspect = ceiling_x / static_cast<float>(ceiling_y);
inline int segment_size = 0;
inline std::unordered_map<int, std::unique_ptr<Segment>> segments = {};

// button 
static const RGB button_border = { 128, 128, 200 };
static const RGBA button_bg = { 128, 128, 200, 20 };
static const RGBA button_bg_hover = { 128, 128, 200, 50 };
static const Vector2<int> button_pos = { 20, 90 };
static const Vector2<int> button_size = { 110, 30 };
inline bool bIsCursorOverButton = false;


static const float ZERO_TOLERANCE = 0.0000001f;
static const float _2PI = static_cast<float>(M_PI) * 2.f;

inline std::map<int, std::unique_ptr<Star>> universe = {}; // all stars
inline std::vector<std::vector<std::pair<int, int>>> constellations = {};

inline SDL_Texture* star_texture = NULL;
inline SDL_Texture* ui_texture = NULL;
inline SDL_Rect star_rect = SDL_Rect{ 0,0,0,0 };
