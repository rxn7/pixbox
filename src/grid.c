#include "grid.h"

#include <stdlib.h>

void grid_init(Cells cells) {
    for(uint32_t x = 0; x < GRID_WIDTH; ++x) {
        for(uint32_t y = 0; y < GRID_HEIGHT; ++y) {
            grid_spawn_cell(cells, x, y, ID_EMPTY);
        }
    }
}

void grid_move_cell(Cells cells, uint8_t from_x, uint8_t from_y, uint8_t to_x, uint8_t to_y) {
    const Cell *from = &cells[from_x][from_y];
    Cell *to = &cells[to_x][to_y];

    to->material_id = from->material_id;
    to->color_idx = from->color_idx;
    to->updated = true;

    grid_delete_cell(cells, from_x, from_y);
}

void grid_spawn_cell(Cells cells, uint8_t x, uint8_t y, MaterialID material_id) {
    Cell *cell = &cells[x][y];
    cell->material_id = material_id;
    cell->color_idx = rand() % 8;
}

void grid_delete_cell(Cells cells, uint8_t x, uint8_t y) {
    Cell *cell = &cells[x][y];
    cell->material_id = ID_EMPTY;
}
