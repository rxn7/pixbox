#include "grid.h"

#include <stdio.h>
#include <stdlib.h>

void grid_init(Cells cells) {
	for(uint32_t y = 0; y < GRID_HEIGHT; ++y) {
		for(uint32_t x = 0; x < GRID_WIDTH; ++x) {
			set_cell(cells, (Point){ x, y }, ID_EMPTY);
		}
	}
}

void move_cell(Cells cells, const Point p1, const Point p2) {
	Cell *from = CELL_AT_POINT(cells, p1);
	Cell *to = CELL_AT_POINT(cells, p2);

	Cell tmp = *from;
	*from = *to;
	*to = tmp;
}

void set_cell(Cells cells, const Point p, const MaterialID material_id) {
	Cell *cell = CELL_AT_POINT(cells, p);
	if(cell->material_id == material_id) {
		return;
	}
	cell->material_id = material_id;
	cell->color_idx = rand() % 8;
}
