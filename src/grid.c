#include "grid.h"

#include <stdlib.h>

void grid_init(Cells cells) {
	for(uint32_t y = 0; y < GRID_HEIGHT; ++y) {
		for(uint32_t x = 0; x < GRID_WIDTH; ++x) {
			spawn_cell(cells, (Point){x, y}, ID_EMPTY);
		}
	}
}

Cell *move_cell(Cells cells, Point p1, Point p2) {
	Cell *from = cell_at(cells, p1);
	Cell *to = cell_at(cells, p2);

	Cell tmp = *from;
	*from = *to;
	*to = tmp;

	return to;
}

Cell *spawn_cell(Cells cells, Point p, MaterialID material_id) {
	Cell *cell = cell_at(cells, p);
	cell->material_id = material_id;
	cell->color_idx = rand() % 8;
	return cell;
}

void delete_cell(Cells cells, Point p) {
	cells[p.y][p.x] = (Cell){.material_id = ID_EMPTY, .color_idx = 0};
}
