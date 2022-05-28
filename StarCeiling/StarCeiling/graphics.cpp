#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

#include "graphics.h"


RGB hsl_to_rgb(const HSL hsl) {
	RGB rgb = { 0, 0, 0 };

	if (hsl.H > 360.f || hsl.H < 0.f || hsl.S>100.f || hsl.S < 0.f || hsl.L>100.f || hsl.L < 0.f) {
		return rgb;
	}

	float s = hsl.S / 100.f;
	float v = hsl.L / 100.f;
	float C = s * v;
	float X = C * (1.f - static_cast<float>(abs(fmod(hsl.H / 60.f, 2) - 1.f)));
	float m = v - C;
	float r, g, b;

	if (hsl.H >= 0.f && hsl.H < 60.f) {
		r = C, g = X, b = 0.f;
	}
	else if (hsl.H >= 60.f && hsl.H < 120.f) {
		r = X, g = C, b = 0.f;
	}
	else if (hsl.H >= 120.f && hsl.H < 180.f) {
		r = 0.f, g = C, b = X;
	}
	else if (hsl.H >= 180.f && hsl.H < 240.f) {
		r = 0.f, g = X, b = C;
	}
	else if (hsl.H >= 240.f && hsl.H < 300.f) {
		r = X, g = 0.f, b = C;
	}
	else {
		r = C, g = 0.f, b = X;
	}

	unsigned char R = static_cast<unsigned char>(roundf((r + m) * 255.f));
	unsigned char G = static_cast<unsigned char>(roundf((g + m) * 255.f));
	unsigned char B = static_cast<unsigned char>(roundf((b + m) * 255.f));

	rgb = { R, G, B };
	return rgb;
}

HSL rgb_to_hsl(const RGB rgb) {
	HSL hsl = { 0.f, 0.f, 0.f };

	float r = rgb.R / 255.f;
	float g = rgb.G / 255.f;
	float b = rgb.B / 255.f;

	float rgb_max = std::max({ r, g, b });
	float rgb_min = std::min({ r, g, b });

	hsl.H = 0.f;
	hsl.L = 50.f * (rgb_min + rgb_max);
	hsl.S = 0.f;

	if (rgb_min == rgb_max) {
		hsl.S = 0.f;
		hsl.H = 0.f;
		return hsl;
	}
	else if (hsl.L < 50.f) {
		hsl.S = 100.f * (rgb_max - rgb_min) / (rgb_max + rgb_min);
	}
	else {
		hsl.S = 100.f * (rgb_max - rgb_min) / (2.f - rgb_max - rgb_min);
	}

	if (rgb_max == r) {
		hsl.H = 60.f * (g - b) / (rgb_max - rgb_min);
	}
	else if (rgb_max == g) {
		hsl.H = 60.f * (b - r) / (rgb_max - rgb_min) + 120.f;
	}
	else if (rgb_max == b) {
		hsl.H = 60.f * (r - g) / (rgb_max - rgb_min) + 240.f;
	}

	if (hsl.H < 0.f)
	{
		hsl.H = hsl.H + 360.f;
	}

	return hsl;
}