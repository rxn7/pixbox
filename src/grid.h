#pragma once

#include "cell.h"

void grid_init(Cells cells);
Cell *grid_move_cell(Cells cells, uint16_t from_x, uint16_t from_y, uint16_t to_x, uint16_t to_y);
Cell *grid_spawn_cell(Cells cells, uint16_t x, uint16_t y, MaterialID material_id);
Cell *grid_delete_cell(Cells cells, uint16_t x, uint16_t y);
