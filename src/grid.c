#include "grid.h"

#include <stdlib.h>

void grid_init(Cells cells) {
	for(uint32_t y = 0; y < GRID_HEIGHT; ++y) {
		for(uint32_t x = 0; x < GRID_WIDTH; ++x) {
			grid_spawn_cell(cells, x, y, ID_EMPTY);
		}
	}
}

Cell *grid_move_cell(Cells cells, uint16_t from_x, uint16_t from_y, uint16_t to_x, uint16_t to_y) {
	Cell *from = &cells[from_y][from_x];
	Cell *to = &cells[to_y][to_x];
	Cell tmp = *from;
	*from = *to;
	*to = tmp;
	return to;
}

Cell *grid_spawn_cell(Cells cells, uint16_t x, uint16_t y, MaterialID material_id) {
	Cell *cell = &cells[y][x];
	cell->material_id = material_id;
	cell->color_idx = rand() % 8;
	return cell;
}

Cell *grid_delete_cell(Cells cells, uint16_t x, uint16_t y) {
	Cell *cell = &cells[y][x];
	cell->material_id = ID_EMPTY;
	return cell;
}
