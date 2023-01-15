#pragma once

#include <vector>
#include "types.h"

#pragma warning(push, 0)
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#pragma warning(pop)
#undef main

class Segment
{
private:
	struct StarData {
		int id=-1;
		Vector2<float> screen_coords { 0.f, 0.f };
		StarSize star_size = StarSize::NONE;
	};

	std::vector<StarData> star_data_{};
	int id_{};
	Vector2<int> coords_ { 0, 0 };
public:
	Segment(int id, Vector2<int> coordinates);

	// ID
	int GetID() { return id_; }

	// Coordinates
	Vector2<int> GetCoordinates() { return coords_; }

	// Stars
	void AddStar(int id, Vector2<float> screen_coordinates, StarSize size);
	void ClearStars();

	// Texture
	SDL_Texture* CreateTexture();
};

