#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <memory>
#include <iostream>

#include "globals.h"
#include "utilities.h"
#include "graphics.h"
#include "star.h"

SDL_Texture* renderText(const std::string& text, eFontSize size, uint16_t x, uint16_t y, bool center) {
	if (!Environment::bFontLoaded) return nullptr; // in case fonts didn't load

	SDL_Texture* font_texture = nullptr;
	SDL_Color foreground = { 128, 128, 200 };

	// select appropriate font for size
	TTF_Font* font = nullptr;
	switch (size) {
	case eFontSize::SMALL:
		font = Environment::font_small;
		break;
	case eFontSize::MEDIUM:
		font = Environment::font_medium;
		break;
	case eFontSize::LARGE:
		font = Environment::font_large;
		break;
	case eFontSize::TITLE:
		font = Environment::font_title;
		break;
	}

	if (text.length() > 0) {
		SDL_Surface* text_surf = nullptr;
		if (Environment::bUseBlendedFonts) {
			text_surf = TTF_RenderText_Blended(font, text.c_str(), foreground);
		}
		else {
			text_surf = TTF_RenderText_Solid(font, text.c_str(), foreground);
		}

		if (!text_surf) {
			std::cout << "Failed to render TTF font: " << SDL_GetError() << "\n";
			return nullptr;
		}

		font_texture = SDL_CreateTextureFromSurface(Environment::renderer, text_surf);
		if (!font_texture) {
			SDL_FreeSurface(text_surf);
			std::cout << "Failed to texture for TTF font: " << SDL_GetError() << "\n";
			return nullptr;
		}

		SDL_Rect dest = SDL_Rect{ x, y, text_surf->w, text_surf->h };
		if (center) dest.x -= static_cast<int>(text_surf->w / 2.0f); // centered
		SDL_RenderCopy(Environment::renderer, font_texture, NULL, &dest);

		SDL_DestroyTexture(font_texture);
		SDL_FreeSurface(text_surf);
	}

	return font_texture;
}

bool renderRect(const Vector2<int> start, const Vector2<int> end, const RGB& color) {
	return renderRect(start, end, RGBA{ color.R, color.G, color.B, SDL_ALPHA_OPAQUE });
}

bool renderRect(const Vector2<int> start, const Vector2<int> end, const RGBA& color) {
	// Draw a rectangle
	//---

	SDL_Rect rect = SDL_Rect{ start.x, start.y, end.x, end.y };

	// store current colour
	RGBA current_color{};
	SDL_GetRenderDrawColor(Environment::renderer, &current_color.R, &current_color.G, &current_color.B, &current_color.A);
	// set given colour
	SDL_SetRenderDrawColor(Environment::renderer, color.R, color.G, color.B, color.A);
	// draw
	int ret = SDL_RenderDrawRect(Environment::renderer, &rect);
	// restore original colour
	SDL_SetRenderDrawColor(Environment::renderer, current_color.R, current_color.G, current_color.B, current_color.A);

	// error handling
	if (ret != 0)
	{
		const char* error = SDL_GetError();
		if (*error != '\0')
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not renderRect. SDL Error: %s at line #%d of file %s/n", error, __LINE__, __FILE__);
			SDL_ClearError();
		}
		return false;
	}

	return true;
}

bool renderLine(const Vector2<int> start, const Vector2<int> end, const RGB& color) {
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

	// error handling
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

bool renderLine(const Vector2<float> start, const Vector2<float> end, const RGB& color) {
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

	// error handling
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

void renderCircle(const Vector2<int> center, float radius, const RGB& color, unsigned int sides) {
	if (sides == 0)
	{
		sides = static_cast<unsigned int>(round(_2PI * radius / 2));
	}

	float d_a = _2PI / sides,
		angle = d_a;

	Vector2<int> start, end;
	end.x = static_cast<long>(radius);
	end.y = static_cast<long>(0.0);
	end = end + center;

	for (int i = 0; i != sides; i++)
	{
		start = end;
		end.x = static_cast<long>(cos(angle) * radius);
		end.y = static_cast<long>(sin(angle) * radius);
		end = end + center;
		angle += d_a;
		renderLine(start, end, color);
	}
}

void drawStars() {
	if (!star_texture) {
		star_texture = SDL_CreateTexture(Environment::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, ceiling_size.x, ceiling_size.y);
		if (!star_texture) {
			std::cout << "Error creating star texture: " << SDL_GetError() << "\n";
			return;
		}
	}
	
	// draw to the texture
	SDL_SetRenderTarget(Environment::renderer, star_texture);

	// fill surface with black
	SDL_SetRenderDrawColor(Environment::renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(Environment::renderer, NULL);

	float screen_coefficient = static_cast<float>(std::min(WINDOW_WIDTH, WINDOW_HEIGHT) * window_scale * zoom);

	// empty segment collections
	

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
			// if (screencoordsInBounds(screen_coords, star->GetZ())) {
			if (true) {
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
}