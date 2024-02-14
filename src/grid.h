#pragma once

#include "cell.h"

typedef Cell Cells[GRID_WIDTH][GRID_HEIGHT];

void grid_init(Cells cells);
void grid_move_cell(Cells cells, uint8_t from_x, uint8_t from_y, uint8_t to_x, uint8_t to_y);
void grid_spawn_cell(Cells cells, uint8_t x, uint8_t y, MaterialID material_id);
void grid_delete_cell(Cells cells, uint8_t x, uint8_t y);
