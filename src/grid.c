#include "grid.h"

#include <stdlib.h>

void grid_init(Cells cells) {
	for(uint32_t y = 0; y < GRID_HEIGHT; ++y) {
		for(uint32_t x = 0; x < GRID_WIDTH; ++x) {
			grid_create_cell(cells, (Point){ x, y }, ID_EMPTY);
		}
	}
}

void grid_move_cell(Cells cells, const Point p1, const Point p2) {
	if(point_equals(p1, p2)) {
		return;
	}

	Cell *from = CELL_AT_POINT(cells, p1);
	Cell *to = CELL_AT_POINT(cells, p2);

	Cell tmp = *from;
	*from = *to;
	*to = tmp;
}

void grid_create_cell(Cells cells, const Point p, const MaterialID material_id) {
	Cell *const cell = CELL_AT_POINT(cells, p);
	cell->material_id = material_id;

	if(material_id == ID_EMPTY) {
		return;
	}

	const Material *const material = material_from_id(material_id);

	switch(material->type) {
		case GAS:
			cell->gas = (GasData){
				.age = 0,
				.death_age = rand() % (material->gas.max_death_age - material->gas.min_death_age) + material->gas.min_death_age,
			};
			break;
		case SOLID:
			cell->solid = (SolidData){};
			break;
		case FLUID:
			cell->fluid = (FluidData){};
			break;
		default:
			break;
	}

	cell->color_idx = rand() % 8;
}

void grid_fill_circle(Cells cells, const Point p, const MaterialID material_id, const int radius) {
	const uint16_t diameter = radius * 2;

	PointComponent x = radius - 1;
	PointComponent y = 0;
	PointComponent dx = 1;
	PointComponent dy = 1;
	PointComponent error = dx - diameter;

	while(x >= y) {
		for(PointComponent i = -x; i < x; ++i) {
			grid_set_cell_safe(cells, (Point){ p.x + i, p.y + y }, material_id);
			grid_set_cell_safe(cells, (Point){ p.x - i, p.y - y }, material_id);
			grid_set_cell_safe(cells, (Point){ p.x + y, p.y + i }, material_id);
			grid_set_cell_safe(cells, (Point){ p.x - y, p.y - i }, material_id);
		}

		grid_set_cell_safe(cells, (Point){ p.x + x, p.y + y }, material_id);
		grid_set_cell_safe(cells, (Point){ p.x + y, p.y + x }, material_id);
		grid_set_cell_safe(cells, (Point){ p.x - y, p.y + x }, material_id);
		grid_set_cell_safe(cells, (Point){ p.x - x, p.y + y }, material_id);
		grid_set_cell_safe(cells, (Point){ p.x - x, p.y - y }, material_id);
		grid_set_cell_safe(cells, (Point){ p.x - y, p.y - x }, material_id);
		grid_set_cell_safe(cells, (Point){ p.x + y, p.y - x }, material_id);
		grid_set_cell_safe(cells, (Point){ p.x + x, p.y - y }, material_id);

		if(error <= 0) {
			++y;
			error += dy;
			dy += 2;
		}

		if(error > 0) {
			--x;
			dx += 2;
			error += dx - diameter;
		}
	}
}
