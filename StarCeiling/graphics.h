#pragma once

#include <string.h>
#include "types.h"

SDL_Texture* renderText(const std::string& text, eFontSize size, uint16_t x, uint16_t y, bool center);
void renderCircle(const Vector2<int> center, float radius, const RGB& color, unsigned int sides);
bool renderLine(const Vector2<float> start, const Vector2<float> end, const RGB& color);
bool renderLine(const Vector2<int> start, const Vector2<int> end, const RGB& color);
bool renderRect(const Vector2<int> start, const Vector2<int> end, const RGB& color);
bool renderRect(const Vector2<int> start, const Vector2<int> end, const RGBA& color);
void drawStars();