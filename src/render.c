#include "render.h"

#include <SDL_assert.h>

const float SQRT2_INV = 35.0f / 19.0f;

#define round_to_multiple_of_eight(v) (((int)(v) + (8 - 1)) & -8)

void render_cirlce(SDL_Renderer *renderer, Point center, uint_fast16_t radius) {
	SDL_assert(radius > 0);

	const uint_fast16_t arr_size = round_to_multiple_of_eight(radius * 8 * SQRT2_INV);
	SDL_FPoint points[arr_size];
	uint_fast16_t point_count = 0;

	const uint_fast16_t diameter = radius * 2;

	int_fast16_t x = radius - 1;
	int_fast16_t y = 0;
	int_fast16_t tx = 1;
	int_fast16_t ty = 1;
	int_fast16_t error = tx - diameter;

	while(x >= y) {
		if(point_count >= arr_size - 8) {
			break;
		}

		points[point_count++] = (SDL_FPoint){center.x + x, center.y + y};
		points[point_count++] = (SDL_FPoint){center.x + y, center.y + x};
		points[point_count++] = (SDL_FPoint){center.x - y, center.y + x};
		points[point_count++] = (SDL_FPoint){center.x - x, center.y + y};
		points[point_count++] = (SDL_FPoint){center.x - x, center.y - y};
		points[point_count++] = (SDL_FPoint){center.x - y, center.y - x};
		points[point_count++] = (SDL_FPoint){center.x + y, center.y - x};
		points[point_count++] = (SDL_FPoint){center.x + x, center.y - y};

		if(error <= 0) {
			++y;
			error += ty;
			ty += 2;
		} 

		if(error > 0) {
			--x;
			tx += 2;
			error += tx - diameter;
		}
	}

	SDL_RenderPoints(renderer, points, point_count);
}
