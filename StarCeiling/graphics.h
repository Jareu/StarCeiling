#pragma once

struct RGB {
	uint8_t R = 0;
	uint8_t G = 0;
	uint8_t B = 0;
};

struct HSL {
	float H = 0;
	float S = 0;
	float L = 0;
};

RGB hsl_to_rgb(const HSL hsl);
HSL rgb_to_hsl(const RGB rgb);