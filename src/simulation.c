#include "simulation.h"
#include "game_context.h"
#include "point.h"

#include <stdatomic.h>

#include <SDL_timer.h>

#include <stdio.h>
#include <stdlib.h>

static const uint32_t TARGET_STEPS_PER_SECOND = 120;
static const float SECONDS_PER_STEP = 1.0f / TARGET_STEPS_PER_SECOND;

static int32_t simulation_loop(void *data);
static void handle_queued_action(GameContext *const ctx, Cells cells);

void simulation_init(Simulation *const sim, GameContext *const ctx) {
	sim->ctx = ctx;
	sim->thread = SDL_CreateThread(simulation_loop, "simulation", sim);
	sim->ctx->performance_stats.target_simulation_step_times_ns = 1000000000 / TARGET_STEPS_PER_SECOND;
}

void simulation_destroy(Simulation *const sim) {
	SDL_WaitThread(sim->thread, nullptr);
}

static Point simulate_gas(const Cells cells, const Point p) {
	return p;
}

static Point simulate_powder(const Cells cells, const Point p) {
	if(p.y >= GRID_HEIGHT - 1) {
		return p;
	}

	Point new_point = (Point){ p.x, p.y + 1 };
	const Cell *const cell_below = CELL_AT_POINT(cells, new_point);

	if(cell_below->material_id == ID_EMPTY || cell_below->material_id == ID_WATER) {
		return new_point;
	}

	int8_t offset = (int)(round(rand() / (float)RAND_MAX) * 2 - 1);
	for(uint8_t i = 0; i < 2; ++i) {
		new_point = (Point){ p.x + offset, p.y + 1 };
		const MaterialID new_point_material_id = CELL_AT_POINT(cells, new_point)->material_id;
		if(p.x + offset >= 0 && p.x + offset < GRID_WIDTH && (new_point_material_id == ID_EMPTY || new_point_material_id == ID_WATER)) {
			return new_point;
		}

		offset *= -1;
	}

	return p;
}

static Point simulate_fluid(Cells cells, const Point p) {
	if(p.y >= GRID_HEIGHT - 1) {
		return p;
	}

	Point new_point = (Point){ p.x, p.y + 1 };
	const Cell *const cell_below = CELL_AT_POINT(cells, new_point);

	if(cell_below->material_id == ID_EMPTY) {
		return new_point;
	}

	int8_t offset = (int)(round(rand() / (float)RAND_MAX) * 2 - 1);
	for(uint8_t i = 0; i < 2; ++i) {
		new_point = (Point){ p.x + offset, p.y };
		const MaterialID new_point_material_id = CELL_AT_POINT(cells, new_point)->material_id;

		if(p.x + offset >= 0 && p.x + offset < GRID_WIDTH && new_point_material_id == ID_EMPTY) {
			return new_point;
		}

		offset *= -1;
	}

	for(uint8_t i = 0; i < 2; ++i) {
		new_point = (Point){ p.x + offset, p.y };
		const MaterialID new_point_material_id = CELL_AT_POINT(cells, new_point)->material_id;

		if(p.x + offset >= 0 && p.x + offset < GRID_WIDTH && new_point_material_id == ID_EMPTY) {
			return new_point;
		}

		offset *= -1;
	}

	return p;
}

int32_t simulation_loop(void *data) {
	Simulation *sim = (Simulation *)data;
	GameContext *ctx = sim->ctx;

	Cells next_step_cells;
	bool odd_step = false;
	bool update_map[GRID_HEIGHT][GRID_WIDTH];

	while(ctx->is_window_open) {
		const uint64_t step_start_tick = SDL_GetPerformanceCounter();
		const uint64_t next_step_tick = step_start_tick + (uint64_t)(SECONDS_PER_STEP * SDL_GetPerformanceFrequency());

		memset(update_map, 0, sizeof(update_map));

		SDL_LockMutex(ctx->cells_mutex);
		memcpy(next_step_cells, ctx->cells, sizeof(Cells));

		if(sim->queue_clear_cells) {
			for(uint32_t y = 0; y < GRID_HEIGHT; ++y) {
				for(uint32_t x = 0; x < GRID_WIDTH; ++x) {
					set_cell(next_step_cells, (Point){ x, y }, ID_EMPTY);
				}
			}
			sim->queue_clear_cells = false;
		}

		handle_queued_action(ctx, next_step_cells);

		if(ctx->is_paused) {
			goto end_step;
		}

		for(PointComponent y = GRID_HEIGHT - 1; y >= 0; y--) {
			for(PointComponent i = 0; i < GRID_WIDTH; ++i) {
				PointComponent x = i;
				// Fix patternization
				if(odd_step) {
					x = GRID_WIDTH - x - 1;
				}

				if(update_map[y][x]) {
					continue;
				}

				Point point = { x, y };
				const Cell *cell = CELL_AT_POINT(ctx->cells, point);

				if(cell->material_id == ID_EMPTY) {
					continue;
				}

				const Material *material = material_from_id(cell->material_id);

				Point new_point = point;
				switch(material->type) {
				case SOLID:
					break;
				case FLUID:
					new_point = simulate_fluid(ctx->cells, point);
					break;
				case POWDER:
					new_point = simulate_powder(ctx->cells, point);
					break;
				case GAS:
					new_point = simulate_gas(ctx->cells, point);
					break;
				default:
					break;
				}

				if(!point_compare(point, new_point)) {
					move_cell(next_step_cells, point, new_point);
					update_map[new_point.y][new_point.x] = true;
				}

				update_map[point.y][point.x] = true;
			}
		}

	end_step:;
		memcpy(ctx->cells, next_step_cells, sizeof(Cells));
		SDL_UnlockMutex(ctx->cells_mutex);

		const uint64_t step_end_tick = SDL_GetPerformanceCounter();

		if(step_end_tick < next_step_tick) {
			const uint32_t sleep_time = ((float)(next_step_tick - step_end_tick) / SDL_GetPerformanceFrequency()) * 1000;
			SDL_Delay(sleep_time);
		}

		ctx->performance_stats.simulation_step_time_ns = (float)(step_end_tick - step_start_tick) / SDL_GetPerformanceFrequency() * 1000000000;
		odd_step = !odd_step;
	}

	return 0;
}

static void handle_queued_action(GameContext *const ctx, Cells cells) {
	if(ctx->queued_action == ACTION_NONE || ctx->hovered_cell == nullptr) {
		return;
	}

	MaterialID material_id;
	switch((GameAction)ctx->queued_action) {
	case ACTION_SPAWN:
		material_id = ctx->selected_material_id;
		break;
	default:
	case ACTION_ERASE:
		material_id = ID_EMPTY;
		break;
	}

	const uint16_t radius = ctx->brush_size;
	const uint16_t diameter = radius * 2;

	PointComponent x = radius - 1;
	PointComponent y = 0;
	PointComponent dx = 1;
	PointComponent dy = 1;
	PointComponent error = dx - diameter;

	while(x >= y) {
		for(PointComponent i = -x; i < x; ++i) {
			set_cell_safe(cells, (Point){ ctx->hovered_x + i, ctx->hovered_y + y }, material_id);
			set_cell_safe(cells, (Point){ ctx->hovered_x - i, ctx->hovered_y - y }, material_id);
			set_cell_safe(cells, (Point){ ctx->hovered_x + y, ctx->hovered_y + i }, material_id);
			set_cell_safe(cells, (Point){ ctx->hovered_x - y, ctx->hovered_y - i }, material_id);
		}

		set_cell_safe(cells, (Point){ ctx->hovered_x + x, ctx->hovered_y + y }, material_id);
		set_cell_safe(cells, (Point){ ctx->hovered_x + y, ctx->hovered_y + x }, material_id);
		set_cell_safe(cells, (Point){ ctx->hovered_x - y, ctx->hovered_y + x }, material_id);
		set_cell_safe(cells, (Point){ ctx->hovered_x - x, ctx->hovered_y + y }, material_id);
		set_cell_safe(cells, (Point){ ctx->hovered_x - x, ctx->hovered_y - y }, material_id);
		set_cell_safe(cells, (Point){ ctx->hovered_x - y, ctx->hovered_y - x }, material_id);
		set_cell_safe(cells, (Point){ ctx->hovered_x + y, ctx->hovered_y - x }, material_id);
		set_cell_safe(cells, (Point){ ctx->hovered_x + x, ctx->hovered_y - y }, material_id);

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
