#pragma once

#include <math.h>
#include <stdint.h>

typedef struct {
	int16_t x;
	int16_t y;
} Point;

inline bool point_is_valid(const Point p) {
	return p.x > 0 && p.x < GRID_WIDTH && p.y > 0 && p.y < GRID_HEIGHT;
}

inline float point_distance(const Point a, const Point b) {
	return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}
