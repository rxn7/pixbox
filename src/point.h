#pragma once

#include <stdint.h>

typedef struct {
	uint16_t x;
	uint16_t y;
} Point;

inline bool is_point_valid(Point *p) {
	return p->x < GRID_WIDTH && p->y < GRID_HEIGHT;
}
