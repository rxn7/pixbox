#pragma once

#include "cell.h"
#include "point.h"

#define CELL_AT_XY(cells, x, y) (&(cells)[y][x])
#define CELL_AT_POINT(cells, p) CELL_AT_XY(cells, p.x, p.y)

void grid_init(Cells cells);
void move_cell(Cells cells, const Point from, const Point to);
void set_cell(Cells cells, const Point p, const MaterialID material_id);

inline void set_cell_safe(Cells cells, const Point p, const MaterialID material_id) {
	if(!point_is_valid(p)) {
		return;
	}

	set_cell(cells, p, material_id);
}
