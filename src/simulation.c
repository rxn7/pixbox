#include "simulation.h"
#include "game_context.h"
#include "point.h"

#include <stdatomic.h>

#include <SDL_timer.h>

#include <stdio.h>
#include <stdlib.h>

static const uint32_t UPDATES_PER_SECOND = 60;
static const float SECONDS_PER_UPDATE = 1.0f / UPDATES_PER_SECOND;

static void handle_queued_action(GameContext *ctx) {
	if(ctx->queued_action == ACTION_NONE || ctx->hovered_cell == nullptr) {
		return;
	}

	for(int32_t x_offset = -ctx->brush_size; x_offset < ctx->brush_size; ++x_offset) {
		for(int32_t y_offset = -ctx->brush_size; y_offset < ctx->brush_size; ++y_offset) {
			const Point p = { ctx->hovered_x + x_offset, ctx->hovered_y + y_offset };

			if(!point_is_valid(p)) {
				continue;
			}

			const float distance = point_distance(p, (Point){ ctx->hovered_x, ctx->hovered_y });
			if(distance > ctx->brush_size) {
				continue;
			}

			switch((GameAction)ctx->queued_action) {
			case ACTION_SPAWN:
				spawn_cell(ctx->cells, p, ctx->selected_material);
				break;

			case ACTION_ERASE:
				spawn_cell(ctx->cells, p, ID_EMPTY);
				break;

			default:
				break;
			}
		}
	}
}

static Point simulate_gas(Cells cells, Point p) {
	if(p.y == 0) {
		return p;
	}

	if(cells[p.y - 1][p.x].material_id == ID_EMPTY) {
		const Point new_point = (Point){ p.x, p.y - 1 };
		move_cell(cells, p, new_point);
		return new_point;
	}

	int8_t offset = (int)(round(rand() / (float)RAND_MAX) * 2 - 1);
	for(uint8_t i = 0; i < 2; ++i) {
		if(p.x + offset >= 0 && p.x + offset < GRID_WIDTH) {
			if(cells[p.y - 1][p.x + offset].material_id == ID_EMPTY) {
				const Point new_point = (Point){ p.x + offset, p.y - 1 };
				move_cell(cells, p, new_point);
				return new_point;
			}

			if(cells[p.y][p.x + offset].material_id == ID_EMPTY) {
				const Point new_point = (Point){ p.x + offset, p.y };
				move_cell(cells, p, new_point);
				return new_point;
			}
		}
	}

	return p;
}

static Point simulate_powder(Cells cells, Point p) {
	if(p.y >= GRID_HEIGHT - 1) {
		return p;
	}

	if(cells[p.y + 1][p.x].material_id == ID_EMPTY) {
		Point new_point = (Point){ p.x, p.y + 1 };
		move_cell(cells, p, new_point);
		return new_point;
	}

	int8_t offset = (int)(round(rand() / (float)RAND_MAX) * 2 - 1);
	for(uint8_t i = 0; i < 2; ++i) {
		if(p.x + offset >= 0 && p.x + offset < GRID_WIDTH && (cells[p.y + 1][p.x + offset].material_id == ID_EMPTY)) {
			Point new_point = (Point){ p.x + offset, p.y + 1 };
			move_cell(cells, p, new_point);
			return new_point;
		}

		offset *= -1;
	}

	return p;
}

int32_t simulation_loop(void *data) {
	GameContext *ctx = (GameContext *)data;

	while(ctx->window_open) {
		const uint64_t start_tick = SDL_GetPerformanceCounter();
		SDL_LockMutex(ctx->cells_mutex);

		handle_queued_action(ctx);

		bool update_map[GRID_HEIGHT][GRID_WIDTH] = { 0 };

		for(uint16_t y = 0; y < GRID_HEIGHT; ++y) {
			for(uint16_t x = 0; x < GRID_WIDTH; ++x) {
				if(update_map[y][x]) {
					continue;
				}

				Point point = { x, y };
				Cell *cell = cell_at(ctx->cells, point);

				if(cell->material_id == ID_EMPTY) {
					continue;
				}

				const Material *material = material_from_id(cell->material_id);

				switch(material->type) {
				case SOLID:
					break;
				case FLUID:
					break;
				case POWDER:
					point = simulate_powder(ctx->cells, point);
					break;
				case GAS:
					point = simulate_gas(ctx->cells, point);
					break;
				default:
					break;
				}

				update_map[point.y][point.x] = true;
			}
		}

		SDL_UnlockMutex(ctx->cells_mutex);

		const uint64_t end_tick = SDL_GetPerformanceCounter();
		const float time_elapsed = (end_tick - start_tick) / (float)SDL_GetPerformanceFrequency();

		if(time_elapsed < SECONDS_PER_UPDATE) {
			SDL_Delay((SECONDS_PER_UPDATE - time_elapsed) * 1000);
		}
	}

	return 0;
}
