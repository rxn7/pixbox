#pragma once

#include <math.h>
#include <stdint.h>

typedef int16_t PointComponent;

typedef struct Point {
	PointComponent x;
	PointComponent y;
} Point;

static inline bool point_is_valid(const Point p) {
	return p.x >= 0 && p.x < GRID_WIDTH && p.y >= 0 && p.y < GRID_HEIGHT;
}

static inline bool point_equals(const Point a, const Point b) {
	return a.x == b.x && a.y == b.y;
}

static inline float point_distance(const Point a, const Point b) {
	return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}
