#pragma once

#include "cell.h"
#include "point.h"

void grid_init(Cells cells);
void move_cell(Cells cells, Point from, Point to);
void spawn_cell(Cells cells, Point p, MaterialID material_id);
void delete_cell(Cells cells, Point p);

inline Cell *cell_at(Cells cells, Point p) {
	return &cells[p.y][p.x];
}
