
#include <iostream>
#include <string>
#include <stdlib.h>     /* srand, rand */
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <chrono>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <map>
#include <utility>

#include "main.h"
#include "Star.h"
#include "types.h"

namespace Environment {
	SDL_Renderer* renderer;
	SDL_Window* window;
}

bool bIsRunning = false;
bool bStarsChanged = false;
bool bFullscreen = true;
bool bIsActive = false;

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

const int WINDOW_WIDTH_HALF = static_cast<int> (WINDOW_WIDTH / 2);
const int WINDOW_HEIGHT_HALF = static_cast<int> (WINDOW_HEIGHT / 2);

const float SECONDS_IN_A_DAY = 86400.f; // (86400 seconds = 1 day)
float earth_rotation = 0.f; // rotation of earth in radians
float latitude = 0.f; // latitude in radians, negative is south. 
const float EARTH_ROTATION_RATE = 20.f;
const RGB constellation_colour = RGB{ 255, 255, 255 };

// -- pan
Vector2<int> window_offset = { 0, 0 };		// offset from center
Vector2<int> window_pan_offset = { 0, 0 };	// offset from center prior to panning
bool mouse_btn_left = false;			// mouse button state
Vector2<int> cursor_pos = { 0, 0 };		// current cursor position
Vector2<int> cursor_pan_pos = { 0, 0 };	// cursor position when panning started

// -- Zoom
const double window_scale = 0.45;
const unsigned short MIN_ZOOM = 1;
const unsigned short MAX_ZOOM = 50;
const float log_min_zoom = static_cast<float>(log(MIN_ZOOM));
const float log_max_zoom = static_cast<float>(log(MAX_ZOOM));
float zoom = 0.f;
unsigned short zoom_steps = MIN_ZOOM;

std::map<int, std::unique_ptr<Star>> sky;
std::vector<std::vector<std::pair<int, int>>> constellations;
SDL_Texture* star_texture;
SDL_Rect star_rect;

void setLatitude(float degrees) {
	latitude = static_cast<float>(M_PI * (0.5f - degrees / 180));
}

void updateZoom() {
	zoom = exp(log_min_zoom + (log_max_zoom - log_min_zoom) * zoom_steps / (MAX_ZOOM - 1));
	bStarsChanged = true;
}

int main() {
	int SDL_RENDERER_FLAGS = 0;
	int SDL_WINDOW_INDEX = -1;

	// set latitude of Adelaide
	setLatitude(-34.814712f);

	// set initial zoom values 
	updateZoom();

	// Add southern cross (Crux)
	auto crux = std::vector<std::pair<int, int>>();
	addStarsToConstellation(crux, 60530, 60893); // Add Alpha Crux and Gamma Crux
	addStarsToConstellation(crux, 62239, 59565); // Add Beta Crux and Delta Crux
	constellations.push_back(crux);

	int SDL_WINDOW_FLAGS = 0;
	SDL_WINDOW_FLAGS = SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED;

	if (bFullscreen) {
		SDL_WINDOW_FLAGS = SDL_WINDOW_FLAGS | SDL_WINDOW_FULLSCREEN;
	}

	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		// SDL failed. Output error message and exit
		std::cout << "Failed to initialize SDL:" << SDL_GetError() << "\n";
		return EXIT_FAILURE;
	}

	// Create Window
	Environment::window = SDL_CreateWindow("Test Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_FLAGS);
	if (!Environment::window) {
		std::cout << "Failed to create window: " << SDL_GetError() << "\n";
		return EXIT_FAILURE;
	}

	// set window size
	SDL_SetWindowMinimumSize(Environment::window, 100, 100);

	// Create Renderer
	Environment::renderer = SDL_CreateRenderer(Environment::window, SDL_WINDOW_INDEX, SDL_RENDERER_FLAGS);
	if (!Environment::renderer) {
		std::cout << "Failed to create renderer: " << SDL_GetError() << "\n";
		return EXIT_FAILURE;
	}

	// set star texture rectangle
	star_rect = SDL_Rect{ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

	SDL_SetRenderDrawColor(Environment::renderer, 0, 0, 0, 255);
	SDL_SetRenderDrawBlendMode(Environment::renderer, SDL_BLENDMODE_BLEND);
	SDL_RenderClear(Environment::renderer); // initialize backbuffer
	bIsRunning = true; // everything was set up successfully
	bIsActive = true;
	SDL_ShowWindow(Environment::window);

	// load stars
	ReadCSV("star_data.csv", true);
	// ReadCSV("star_data_large.csv", true);

	// create star texture
	uint64_t t_before = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	star_texture = drawStars();
	uint64_t t_after = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	uint64_t search_time = (t_after - t_before);
	std::cout << "Star draw time: " << search_time << " microseconds \n";
	SDL_SetRenderTarget(Environment::renderer, NULL);
	SDL_RenderCopy(Environment::renderer, star_texture, NULL, &star_rect);
	SDL_RenderPresent(Environment::renderer);

	while (bIsRunning) {
		handleEvents();
		handleUserInput();
		update();
		if (bStarsChanged) {
			render();
			bStarsChanged = false;
		}
	}

	// frees memory associated with renderer and window
	SDL_DestroyRenderer(Environment::renderer);
	SDL_DestroyWindow(Environment::window);
	Environment::renderer = NULL;
	Environment::window = NULL;

	SDL_DestroyTexture(star_texture);

	IMG_Quit();
	SDL_Quit();

	return EXIT_SUCCESS;
}

void increment_time(const float delta_seconds) {
	earth_rotation = fmod(earth_rotation + _2PI * delta_seconds / SECONDS_IN_A_DAY, _2PI);
}

void addStarsToConstellation(std::vector<std::pair<int, int>>& constellation, const int star_a, const int star_b) {
	constellation.push_back(std::pair<int, int>(star_a, star_b));
}

Vector3<float> Rotate_X(const Vector3<float>& v, float angle) {
	float cosa = cos(angle);
	float sina = sin(angle);
	Vector3<float> new_loc = {};
	new_loc.x = v.x;
	new_loc.y = v.y * cosa - v.z * sina;
	new_loc.z = v.y * sina + v.z * cosa;
	return new_loc;
}

Vector3<float> Rotate_Y(const Vector3<float>& v, float angle) {
	float cosa = cos(angle);
	float sina = sin(angle);
	Vector3<float> new_loc = {};
	new_loc.x = v.x * cosa + v.z * sina;
	new_loc.y = v.y;
	new_loc.z = v.z * cosa - v.x * sina;
	return new_loc;
}

Vector3<float> Rotate_Z(const Vector3<float>& v, float angle) {
	float cosa = cos(angle);
	float sina = sin(angle);
	Vector3<float> new_loc = {};
	new_loc.x = v.x * cosa - v.y * sina;
	new_loc.y = v.x * sina + v.y * cosa;
	new_loc.z = v.z;
	return new_loc;
}


void update() {
	// do things
	increment_time(EARTH_ROTATION_RATE);

	auto sky_it = sky.begin();
	while (sky_it != sky.end())
	{
		// get value from star
		auto& star = sky_it->second;

		if (star) {
			// rotate around Y axis by time of day, then rotate about X axis by latitude
			Vector3 new_loc = star->GetWorldLocation();
			star->Rotate_Y(new_loc, earth_rotation);
			star->Rotate_X(latitude);
			star->UpdateTransforms();
		}

		sky_it++;
	}

	bStarsChanged = true;
}

// Render the Game
void render() {
	SDL_SetRenderDrawColor(Environment::renderer, 0, 0, 0, 255);
	SDL_RenderClear(Environment::renderer);
	star_texture = drawStars();
	SDL_SetRenderTarget(Environment::renderer, NULL);
	SDL_RenderCopy(Environment::renderer, star_texture, NULL, &star_rect);
	SDL_RenderPresent(Environment::renderer);
}

// handles any events that SDL noticed.
void handleEvents() {
	//the only event we'll check is the  SDL_QUIT event.
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			bIsRunning = false;
			break;
		case SDL_MOUSEWHEEL:
			if (event.wheel.y < 0) {
				// zoom in
				zoom_steps = std::clamp(++zoom_steps, MIN_ZOOM, MAX_ZOOM);
			}
			else if (event.wheel.y > 0) {
				// zoom out
				zoom_steps = std::clamp(--zoom_steps, MIN_ZOOM, MAX_ZOOM);
			}
			updateZoom();
			break;
		case SDL_WINDOWEVENT:
			switch (event.window.event) {
			case SDL_WINDOWEVENT_FOCUS_LOST:

				break;
			case SDL_WINDOWEVENT_FOCUS_GAINED:
			case SDL_WINDOWEVENT_RESTORED:
			case SDL_WINDOWEVENT_SHOWN:
				//case SDL_WINDOWEVENT_EXPOSED:
				bIsActive = true;
				break;
			case SDL_WINDOWEVENT_HIDDEN:
			case SDL_WINDOWEVENT_MINIMIZED:
				bIsActive = false;
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
}

void handleUserInput() {
	// get mouse position and button states
	Uint32 mouse_buttons = SDL_GetMouseState(&cursor_pos.x, &cursor_pos.y);

	if ((mouse_buttons & SDL_BUTTON_LMASK) != 0) {
		// Left button down
		if (!mouse_btn_left) {
			// transition from button up to button down
			window_pan_offset = window_offset;
			cursor_pan_pos = cursor_pos;
		}

		window_offset = window_pan_offset + cursor_pos - cursor_pan_pos;
		bStarsChanged = true;
		mouse_btn_left = true;
	}
	else if ((mouse_buttons & SDL_BUTTON_LMASK) == 0) {

		// left button up
		if (mouse_btn_left) {
			// transition from button down to button up

		}

		mouse_btn_left = false;
	}

	// Get current keystate
	const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
	if (currentKeyStates[SDL_SCANCODE_ESCAPE]) {
		// escape
		bIsRunning = false;
	}

	if (currentKeyStates[SDL_SCANCODE_UP])
	{
		// Key Up 

	}
	else if (currentKeyStates[SDL_SCANCODE_DOWN])
	{
		// Key Down

	}

	if (currentKeyStates[SDL_SCANCODE_LEFT])
	{
		// Key Left

	}
	else if (currentKeyStates[SDL_SCANCODE_RIGHT])
	{
		// Key Right

	}
}


bool renderLine(const Vector2<int> start, const Vector2<int> end, const RGB& color)
{
	RGB oldc;

	int ww, wh;
	SDL_GetWindowSize(Environment::window,
		&ww,
		&wh);
	// Draw a line
	//---
	int ret = SDL_RenderDrawLine(
		Environment::renderer, // SDL_Renderer* renderer: the renderer in which draw
		start.x,               // int x1: x of the starting point
		start.y,          // int y1: y of the starting point
		end.x,                 // int x2: x of the end point
		end.y);           // int y2: y of the end point

	if (ret != 0)
	{
		const char* error = SDL_GetError();
		if (*error != '\0')
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not renderDrawLine. SDL Error: %s at line #%d of file %s/n", error, __LINE__, __FILE__);
			SDL_ClearError();
		}
		return false;
	}
	return true;
}

bool renderLine(const Vector2<float> start, const Vector2<float> end, const RGB& color)
{
	RGB oldc;

	int ww, wh;
	SDL_GetWindowSize(Environment::window,
		&ww,
		&wh);
	// Draw a line
	//---
	int ret = SDL_RenderDrawLine(
		Environment::renderer, // SDL_Renderer* renderer: the renderer in which draw
		static_cast<int>(round(start.x)),               // int x1: x of the starting point
		static_cast<int>(round(start.y)),          // int y1: y of the starting point
		static_cast<int>(round(end.x)),                 // int x2: x of the end point
		static_cast<int>(round(end.y)));           // int y2: y of the end point

	if (ret != 0)
	{
		const char* error = SDL_GetError();
		if (*error != '\0')
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not renderDrawLine. SDL Error: %s at line #%d of file %s/n", error, __LINE__, __FILE__);
			SDL_ClearError();
		}
		return false;
	}
	return true;
}

void renderCircle(const Vector2<int> center, float radius, const RGB& color, unsigned int sides)
{
	if (sides == 0)
	{
		sides = static_cast<unsigned int>(round(_2PI * radius / 2));
	}

	float d_a = _2PI / sides,
		angle = d_a;

	Vector2<int> start, end;
	end.x = radius;
	end.y = 0.0f;
	end = end + center;

	for (int i = 0; i != sides; i++)
	{
		start = end;
		end.x = cos(angle) * radius;
		end.y = sin(angle) * radius;
		end = end + center;
		angle += d_a;
		renderLine(start, end, color);
	}
}

/* Takes a std vector of strings, returns the value at the given index.
Out of bounds exception returns an empty string.
*/
std::string getValueFromIndex(std::vector<std::string>& v, const unsigned int index) {
	return (index >= v.size()) ? "" : trim(v[index]);
}

Vector2<int> getScreenCoords(const float scalar, const Vector2<float>& coords_n) {
	return Vector2<int>(scalar * coords_n.x + WINDOW_WIDTH_HALF, scalar * coords_n.y + WINDOW_HEIGHT_HALF);
}

bool screencoordsInBounds(Vector2<int> screen_coords, float Z) {
	bool x_in_bounds = screen_coords.x > 0 && screen_coords.x < WINDOW_WIDTH;
	bool y_in_bounds = screen_coords.y > 0 && screen_coords.y < WINDOW_HEIGHT;
	bool z_in_bounds = Z > 0.f;
	return (x_in_bounds && y_in_bounds && z_in_bounds);
}

SDL_Texture* drawStars() {
	SDL_Texture* texture = SDL_CreateTexture(Environment::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);

	// draw to the texture
	SDL_SetRenderTarget(Environment::renderer, texture);

	// fill surface with black
	SDL_SetRenderDrawColor(Environment::renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(Environment::renderer, NULL);

	float screen_coefficient = static_cast<float>(std::min(WINDOW_WIDTH, WINDOW_HEIGHT) * window_scale * zoom);

	// draw the star
	auto sky_it = sky.begin();
	while (sky_it != sky.end())
	{
		// get value from star
		auto& star = sky_it->second;

		if (star) {
			// Color
			const uint8_t brightness = star->GetBrightness();
			SDL_SetRenderDrawColor(Environment::renderer, star->GetColour().R, star->GetColour().G, star->GetColour().B, brightness);

			// Screen Coordinates
			const Vector2 screen_coords = getScreenCoords(screen_coefficient, star->GetScreenCoords()) + window_offset;

			// Only draw star if it is on screen
			if (screencoordsInBounds(screen_coords, star->GetZ())) {
				if (star->GetMagnitude() < 2.f) {
					renderCircle(screen_coords, 0.75f, star->GetColour(), 4);
				}
				else {
					SDL_RenderDrawPoint(Environment::renderer, static_cast<int>(round(screen_coords.x)), static_cast<int>(round(screen_coords.y)));
				}

				if (star->GetName() == "Polaris") {
					renderCircle(screen_coords, 2.f, star->GetColour(), 4);
				}
			}
		}

		sky_it++;
	}

	// Draw constellations
	SDL_SetRenderDrawColor(Environment::renderer, constellation_colour.R, constellation_colour.G, constellation_colour.B, 35);

	for (auto& constellation : constellations) {
		for (auto& star_pair : constellation) {
			auto& star_a = sky.find(star_pair.first)->second;
			auto& star_b = sky.find(star_pair.second)->second;
			if (star_a && star_b) {
				// Screen Coordinates
				const Vector2<int> screen_coords_a = getScreenCoords(screen_coefficient, star_a->GetScreenCoords()) + window_offset;
				const Vector2<int> screen_coords_b = getScreenCoords(screen_coefficient, star_b->GetScreenCoords()) + window_offset;

				// Only draw if both stars are on screen
				bool star_a_in_bounds = (screencoordsInBounds(screen_coords_a, star_a->GetZ()));
				bool star_b_in_bounds = (screencoordsInBounds(screen_coords_b, star_b->GetZ()));

				if (star_a_in_bounds != star_b_in_bounds) {
					// only one star is in bounds. interpolate
					// this interpolation function is not linear, it is a projection from 3D cartesian coordinates to spherical coordinates
					// step 1 is to get 3D bounds in XYZ cartesian domain by finding spherical coordinates of all 4 corners of window
					// one plane at Z=0 and one plane at Z=1, both bounded by an edge function

				}
				else if (star_a_in_bounds && star_b_in_bounds) {
					renderLine(screen_coords_a, screen_coords_b, constellation_colour);
				}
			}
		}
	}

	return texture;
}

// Read CSV file into array
void ReadCSV(std::string filename, bool has_header) {
	std::vector<std::string> row;
	std::string line, word;

	std::ifstream file(filename);

	if (!file.good()) {
		std::cout << "Filename \"" << filename << "\" doesn't exist. Current working dir: " << std::filesystem::current_path() << "\n";
		return;
	}

	if (!file.is_open()) {
		std::cout << "Could not open the file\n";
		return;
	}

	// read file

	bool firstrow = true;
	while (getline(file, line))
	{
		row.clear();
		std::stringstream str(line);

		if (firstrow) {
			firstrow = false;
			if (has_header) continue;
		}

		while (getline(str, word, ',')) {
			row.push_back(word);
		}

		// create a new star from values
		auto star = std::unique_ptr<Star>(new Star());
		star->SetID(std::stoi(getValueFromIndex(row, 0)));
		star->SetName(getValueFromIndex(row, 6));
		star->SetMagnitude(getValueFromIndex(row, 13));
		star->SetColourIndex(getValueFromIndex(row, 16));

		float x = std::stof(getValueFromIndex(row, 17));
		float y = std::stof(getValueFromIndex(row, 18));
		float z = std::stof(getValueFromIndex(row, 19));

		star->SetLocation(Vector3{ x, y, z });

		// move star into sky
		if (star->GetMagnitude() < Star::MIN_MAGNITUDE) {
			sky.insert(std::pair<int, std::unique_ptr<Star>>(star->GetID(), std::move(star)));
		}
	}

	file.close();

	std::cout << "Successfully read " << sky.size() << " stars.\n";
}