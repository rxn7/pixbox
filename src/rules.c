#include "rules.h"
#include "game_context.h"

#include <SDL_timer.h>
#include <stdlib.h>

Cell *update_powder(Cells cells, uint16_t x, uint16_t y) {
	Cell *cell = &cells[y][x];

	if(y >= GRID_HEIGHT - 1) {
		return cell;
	}

	if(cells[y + 1][x].material_id == ID_EMPTY || cells[y + 1][x].material_id == cell->material_id) {
		return grid_move_cell(cells, x, y, x, y + 1);
	}

	for(int8_t offset = -1; offset <= 1; offset += 2) {
		if(x + offset >= 0 && x + offset < GRID_WIDTH && (cells[y + 1][x + offset].material_id == ID_EMPTY || cells[y+1][x+offset].material_id == cell->material_id)) {
			return grid_move_cell(cells, x, y, x + offset, y + 1);
		}
	}

	return cell;
}

Cell *update_fluid(Cells cells, uint16_t x, uint16_t y) {
	Cell *cell = update_powder(cells, x, y);
	if(cell) {
		return cell;
	}

	for(int8_t offset = -1; offset <= 1; offset += 2) {
		if(x + offset >= 0 && x + offset < GRID_WIDTH && cells[y][x + offset].material_id == ID_EMPTY) {
			return grid_move_cell(cells, x, y, x + offset, y);
		}
	}

	return &cells[y][x];
}

int32_t handle_rules_thread(void *data) {
	GameContext *ctx = (GameContext *)data;

	while(ctx->window_open) {
		SDL_LockMutex(ctx->cells_mutex);

		if(ctx->spawn_cell_queued) {
			for(int32_t x_offset = -ctx->brush_size; x_offset < ctx->brush_size; ++x_offset) {
				for(int32_t y_offset = -ctx->brush_size; y_offset < ctx->brush_size; ++y_offset) {
					const int32_t x = ctx->hovered_x + x_offset;
					if(x < 0 || x >= GRID_WIDTH) {
						continue;
					}

					const int32_t y = ctx->hovered_y + y_offset;
					if(y < 0 || y >= GRID_HEIGHT) {
						continue;
					}

					grid_spawn_cell(ctx->cells, x, y, ID_SAND);
				}
			}
		}

		for(uint32_t y = 0; y < GRID_HEIGHT; ++y) {
			for(uint32_t x = 0; x < GRID_WIDTH; ++x) {
				Cell *cell = &ctx->cells[y][x];
				cell->updated = false;
			}
		}

		for(uint32_t y = 0; y < GRID_HEIGHT; ++y) {
			for(uint32_t x = 0; x < GRID_WIDTH; ++x) {
				Cell *cell = &ctx->cells[y][x];
				if(cell->updated) {
					continue;
				}

				const Material *material = material_from_id(cell->material_id);
				if(!material) {
					continue;
				}

				switch(material->type) {
				case SOLID:
					break;
				case FLUID:
					cell = update_fluid(ctx->cells, x, y);
					break;
				case POWDER:
					cell = update_powder(ctx->cells, x, y);
					break;
				case GAS:
					break;
				default:
					break;
				}

				cell->updated = true;
			}
		}
		SDL_UnlockMutex(ctx->cells_mutex);

		SDL_Delay(1000 / 60);
	}

	return 0;
}
