#include "render.h"

#include <SDL_assert.h>

const float SQRT2_INV = 35.0f / 19.0f;

#define round_to_multiple_of_eight(v) (((int)(v) + (8 - 1)) & -8)

void render_cirlce(SDL_Renderer *const renderer, const Point center, const uint16_t radius) {
	SDL_assert(radius > 0);

	const uint16_t arr_size = round_to_multiple_of_eight(radius * 8 * SQRT2_INV);
	SDL_FPoint points[arr_size];
	uint16_t point_count = 0;

	const uint16_t diameter = radius * 2;

	int16_t x = radius - 1;
	int16_t y = 0;
	int16_t dx = 1;
	int16_t dy = 1;
	int16_t error = dx - diameter;

	while(x >= y) {
		if(point_count >= arr_size - 8) {
			break;
		}

		points[point_count++] = (SDL_FPoint){ center.x + x, center.y + y };
		points[point_count++] = (SDL_FPoint){ center.x + y, center.y + x };
		points[point_count++] = (SDL_FPoint){ center.x - y, center.y + x };
		points[point_count++] = (SDL_FPoint){ center.x - x, center.y + y };
		points[point_count++] = (SDL_FPoint){ center.x - x, center.y - y };
		points[point_count++] = (SDL_FPoint){ center.x - y, center.y - x };
		points[point_count++] = (SDL_FPoint){ center.x + y, center.y - x };
		points[point_count++] = (SDL_FPoint){ center.x + x, center.y - y };

		if(error <= 0) {
			++y;
			error += dy;
			dy += 2;
		}

		if(error > 0) {
			--x;
			dx += 2;
			error += dx - diameter;
		}
	}

	SDL_RenderPoints(renderer, points, point_count);
}
