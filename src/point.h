#pragma once

#include <math.h>
#include <stdint.h>

typedef struct {
	uint16_t x;
	uint16_t y;
} Point;

inline bool point_is_valid(const Point p) {
	return p.x < GRID_WIDTH && p.y < GRID_HEIGHT;
}

inline float point_distance(const Point a, const Point b) {
	return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}
