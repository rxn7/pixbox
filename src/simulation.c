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
		new_point = (Point){ p.x + offset, p.y + 1 };
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

	while(ctx->is_window_open) {
		const uint64_t start_tick = SDL_GetPerformanceCounter();

		if(ctx->is_paused) {
			goto end_step;
		}

		bool update_map[GRID_HEIGHT][GRID_WIDTH] = { 0 };

		Cells updated_cells;
		SDL_LockMutex(ctx->cells_mutex);
		memcpy(&updated_cells, ctx->cells, sizeof(Cells));
		handle_queued_action(ctx, updated_cells);

		for(PointComponent y = GRID_HEIGHT - 1; y >= 0; y--) {
			for(PointComponent x = 0; x < GRID_WIDTH; ++x) {
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
					move_cell(updated_cells, point, new_point);
					update_map[new_point.y][new_point.x] = true;
				}

				update_map[point.y][point.x] = true;
			}
		}
		memcpy(ctx->cells, &updated_cells, sizeof(Cells));
		SDL_UnlockMutex(ctx->cells_mutex);

	end_step:;
		const uint64_t end_tick = SDL_GetPerformanceCounter();
		const float time_elapsed = (end_tick - start_tick) / (float)SDL_GetPerformanceFrequency();

		if(time_elapsed < SECONDS_PER_STEP) {
			SDL_Delay((SECONDS_PER_STEP - time_elapsed) * 1000);
		}

		ctx->performance_stats.simulation_step_time = time_elapsed * 1000;
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
	case ACTION_ERASE:
		material_id = ID_EMPTY;
		break;
	default:
		break;
	}

	const uint16_t radius = ctx->brush_size;
	const uint16_t diameter = radius * 2;

	PointComponent x = radius - 1;
	PointComponent y = 0;
	PointComponent tx = 1;
	PointComponent ty = 1;
	PointComponent error = tx - diameter;

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
			error += ty;
			ty += 2;
		}

		if(error > 0) {
			--x;
			tx += 2;
			error += tx - diameter;
		}
	}
}
