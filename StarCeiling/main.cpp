
#pragma warning(push, 0)
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#pragma warning(pop)
#undef main

#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <chrono>
#include <fstream>
#include <sstream>
#include <memory>
#include <filesystem>
#include <utility>

#include "main.h"
#include "graphics.h"
#include "utilities.h"
#include "globals.h"
#include "Star.h"
#include "types.h"

inline void setLatitude(float degrees) {
	latitude = static_cast<float>(M_PI * (0.5f - degrees / 180));
}

/*
	Updates ceiling_size with the width and height of a rectangle containing the ceiling in pixels. Should be called when window resizes.
*/
void calculateCeilingSize() {
	float screen_aspect = WINDOW_WIDTH / static_cast<float>(WINDOW_HEIGHT); // width / height, calculated here becuase it may be variable in future.

	if (ceiling_aspect > screen_aspect) {
		// ceiling size
		ceiling_size.x = WINDOW_WIDTH - margin * 2;
		ceiling_size.y = ceiling_size.x * ceiling_y / static_cast<float>(ceiling_x);
		// offset
		ceiling_offset.x = margin;
		ceiling_offset.y = static_cast<int>(std::round((WINDOW_HEIGHT - ceiling_size.y) / 2.0));
	}
	else {
		// ceiling size
		ceiling_size.y = WINDOW_HEIGHT - margin * 2;
		ceiling_size.x = ceiling_size.y * ceiling_x / static_cast<float>(ceiling_y);
		// offset
		ceiling_offset.x = static_cast<int>(std::round((WINDOW_WIDTH - ceiling_size.x) / 2.0));
		ceiling_offset.y = margin;
	}
	
	segment_size = ceiling_size.x / ceiling_x;
}

/*
	Updates ceiling_offset based on window size. Should be called when window resizes.
*/
void calculateCeilingOffset() {
	if (WINDOW_WIDTH > WINDOW_HEIGHT) {
	}
	else {
	}
}

int main() {
	int SDL_RENDERER_FLAGS = 0;
	int SDL_WINDOW_INDEX = -1;

	// set latitude of Adelaide
	setLatitude(-34.814712f);
	updateZoom(); // set initial zoom values 

	populateConstellations();

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

	// load font
	if (TTF_Init() != 0) {
		// SDL failed. Output error message and exit
		std::cout << "Failed to initialize fonts:" << SDL_GetError() << "\n";
		return EXIT_FAILURE;
	}

	Environment::font_small = TTF_OpenFont((Environment::fontname + ".ttf").c_str(), FONT_SIZE_SMALL);
	Environment::font_medium = TTF_OpenFont((Environment::fontname + ".ttf").c_str(), FONT_SIZE_MEDIUM);
	Environment::font_large = TTF_OpenFont((Environment::fontname + ".ttf").c_str(), FONT_SIZE_LARGE);
	Environment::font_title = TTF_OpenFont((Environment::fontname + ".ttf").c_str(), FONT_SIZE_TITLE);

	// check that fonts loaded successfully
	Environment::bFontLoaded = (Environment::font_small && Environment::font_medium && Environment::font_large && Environment::font_title);
	if (!Environment::bFontLoaded) {
		std::string err = SDL_GetError();
		std::cout << "Failed to load font '" << Environment::fontname << ".ttf': " << err << "\n";
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
	calculateCeilingSize();

	std::cout << "Ceiling Size: {" << ceiling_size.x << ", " << ceiling_size.y << "}\n";
	std::cout << "Ceiling Offset: {" << ceiling_offset.x << ", " << ceiling_offset.y << "}\n";
	star_rect = SDL_Rect{ ceiling_offset.x, ceiling_offset.y, ceiling_size.x, ceiling_size.y };

	SDL_SetRenderDrawColor(Environment::renderer, 0, 0, 0, 255);
	SDL_SetRenderDrawBlendMode(Environment::renderer, SDL_BLENDMODE_BLEND);
	SDL_RenderClear(Environment::renderer); // initialize backbuffer
	bIsRunning = true; // everything was set up successfully

	bIsActive = true;
	SDL_ShowWindow(Environment::window);

	// show loading window
	render();

	// load stars
	//readCSV("star_data.csv", true);
	readCSV("star_data_large.csv", true);

	bLoadingStars = false;
	
	correctStarRotation(-M_PI_2);

	/*
	// create star texture
	uint64_t t_before = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	star_texture = drawStars();
	uint64_t t_after = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	uint64_t search_time = (t_after - t_before);
	std::cout << "Star draw time: " << search_time << " microseconds \n";
	SDL_SetRenderTarget(Environment::renderer, NULL);
	SDL_RenderCopy(Environment::renderer, star_texture, NULL, &star_rect);
	SDL_RenderPresent(Environment::renderer);
	*/

	
	while (bIsRunning) {
		handleEvents();
		handleUserInput();
		//update();
		//if (bStarsChanged) {
		render();
		//	bStarsChanged = false;
		//}
	}

	// frees memory associated with renderer and window
	SDL_DestroyRenderer(Environment::renderer);
	SDL_DestroyWindow(Environment::window);
	Environment::renderer = NULL;
	Environment::window = NULL;

	// destroy textures
	SDL_DestroyTexture(star_texture);
	SDL_DestroyTexture(ui_texture);

	// close fonts
	TTF_CloseFont(Environment::font_small);
	TTF_CloseFont(Environment::font_medium);
	TTF_CloseFont(Environment::font_large);
	TTF_CloseFont(Environment::font_title);
	TTF_Quit();

	IMG_Quit();
	SDL_Quit();

	return 0;
}

void update() {
	// do things
	if (EARTH_ROTATION_RATE > 0) {
		increment_time(EARTH_ROTATION_RATE);
		auto universe_i = universe.begin();
		while (universe_i != universe.end())
		{
			// get value from star
			auto& star = universe_i->second;

			if (star) {
				// rotate around Y axis by time of day, then rotate about X axis by latitude
				Vector3 new_loc = star->GetAbsoluteLocation();
				star->Rotate_Y(new_loc, earth_rotation);
				star->Rotate_X(latitude);
				star->UpdateTransforms();
			}

			universe_i++;
		}

		bStarsChanged = true;
	}
}

// Render the Game
void render() {
	if (!bIsActive) return;

	SDL_SetRenderDrawColor(Environment::renderer, 0, 0, 0, 255);
	SDL_RenderClear(Environment::renderer);

	if (bLoadingStars) {
		ui_texture = renderText("LOADING...", eFontSize::TITLE, WINDOW_WIDTH_HALF, WINDOW_HEIGHT_HALF - 30, true);
	}
	else {
		drawStars();

		// draw border
		renderRect(Vector2{ 0, 0 }, ceiling_size, RGBA{ 128, 128, 200, 128 });

		// draw segments
		for (int x = 0; x < ceiling_size.x; x += segment_size) {
			renderLine(Vector2{ x, 0 }, Vector2{ x, ceiling_size.y }, RGB{ 100, 100, 160 });
		}
		for (int y = 0; y < ceiling_size.y; y += segment_size) {
			renderLine(Vector2{ 0, y }, Vector2{ ceiling_size.x, y }, RGB{ 100, 100, 160 });
		}

		ui_texture = renderText("Testing", eFontSize::SMALL, 20, 20, false);
	}

	SDL_SetRenderTarget(Environment::renderer, NULL); // NULL: render to the window
	SDL_RenderCopy(Environment::renderer, star_texture, NULL, &star_rect);
	SDL_RenderCopy(Environment::renderer, ui_texture, NULL, NULL);
	SDL_RenderPresent(Environment::renderer);
}

// handles any events that SDL noticed.
void handleEvents() {
	//the only event we'll check is the  SDL_QUIT event.
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
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
			switch(event.window.event) {
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

void handleUserInput()
{
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

void populateConstellations() {
	// Add southern cross (Crux)
	auto crux = std::vector<std::pair<int, int>>();
	addStarsToConstellation(crux, 60530, 60893); // Add Alpha Crux and Gamma Crux
	addStarsToConstellation(crux, 62239, 59565); // Add Beta Crux and Delta Crux
	constellations.push_back(crux);
}

// Read CSV file into array
void readCSV(std::string filename, bool has_header) {
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

		star->SetAbsoluteLocation(Vector3<float>{ x, y, z });

		// move star into sky
		if (star->GetMagnitude() < Star::MIN_MAGNITUDE) {
			groupStarBySize(*star);
			universe.insert(std::pair<int, std::unique_ptr<Star>>( star->GetID(), std::move(star)) );
		}
	}

	file.close();

	std::cout << "Successfully read " << universe.size() << " stars.\n";

	// sort by star magnitudes
	std::cout << "Sorting stars by magnitude ... ";
	sortStars();
	std::cout << "done.\n";

}