#pragma once

#include "cell.h"
#include "point.h"

#define CELL_AT_XY(cells, x, y) (&(cells)[y][x])
#define CELL_AT_POINT(cells, p) CELL_AT_XY(cells, p.x, p.y)

void grid_init(Cells cells);
void grid_move_cell(Cells cells, const Point from, const Point to);
void grid_create_cell(Cells cells, const Point p, const MaterialID material_id);
void grid_fill_circle(Cells cells, const Point p, const MaterialID material_id, const int radius);

static inline void grid_set_cell_safe(Cells cells, const Point p, const MaterialID material_id) {
	if(!point_is_valid(p)) {
		return;
	}

	grid_create_cell(cells, p, material_id);
}
