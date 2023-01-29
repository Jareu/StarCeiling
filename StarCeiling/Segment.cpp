#include "Segment.h"
#include "globals.h"
#include <iostream>

Segment::Segment(int id, Vector2<int> coordinates) : id_{ id }, coords_{ coordinates }{}

void Segment::AddStar(int id, Vector2<float> screen_coordinates, StarSize size) {
	star_data_.push_back(StarData(id, screen_coordinates, size));
}

void Segment::ClearStars() {
	star_data_.clear();
}

// Texture
SDL_Texture* Segment::CreateTexture() {
	SDL_Texture* segment_texture = nullptr;

	// save renderers target
	SDL_Texture* target = SDL_GetRenderTarget(Environment::renderer);

	// determine segment size
	int segment_size = std::min(ceiling_size.x, ceiling_size.y);

	// create texture
	segment_texture = SDL_CreateTexture(Environment::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, segment_size, segment_size);
	if (!segment_texture) {
		std::cout << "Error creating star texture: " << SDL_GetError() << "\n";
		SDL_SetRenderTarget(Environment::renderer, target);
		return nullptr;
	}


	// set the texture as a target
	SDL_SetRenderTarget(Environment::renderer, star_texture);

	// fill surface with white
	SDL_SetRenderDrawColor(Environment::renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(Environment::renderer, NULL);

	// draw stars

	for (const auto& star : star_data_) {
		// check size
		if (star.star_size == StarSize::NONE) {
			break;
		}

		SDL_SetRenderDrawColor(Environment::renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

		// draw appropriate marker
		switch (star.star_size) {
			case StarSize::SMALL:
			
				break;
			case StarSize::MEDIUM:

				break;
			case StarSize::LARGE:

				break;
		}
	}

	// draw segment ID


	// draw north marker


	// restore renderer target
	SDL_SetRenderTarget(Environment::renderer, target);

	return target;
}