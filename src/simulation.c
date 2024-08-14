#include "simulation.h"
#include "game_context.h"
#include "grid.h"
#include "point.h"

#include <stdatomic.h>
#include <stdlib.h>

#include <SDL_timer.h>

static const uint32_t TARGET_STEPS_PER_SECOND = 120;
static const float SECONDS_PER_STEP = 1.0f / TARGET_STEPS_PER_SECOND;

static int32_t simulation_loop(void *data);
static void simulate_gas(Simulation *const sim, Cell *const cell, const Material *const material, const Point p);
static void simulate_powder(Simulation *const sim, Cell *const cell, const Material *const material, const Point p);
static void simulate_fluid(Simulation *const sim, Cell *const cell, const Material *const material, const Point p);
static void move_cell(Simulation *const sim, const Point from, const Point to);
static void handle_queued_action(GameContext *const ctx, Cells cells);

void simulation_init(Simulation *const sim, GameContext *const ctx) {
	grid_init(ctx->cells);

	sim->ctx = ctx;
	sim->thread = SDL_CreateThread(simulation_loop, "sim", sim);
	sim->ctx->performance_stats.target_simulation_step_times_ns = 1000000000 / TARGET_STEPS_PER_SECOND;
	sim->queue_clear_cells = false;
	sim->queue_step = false;
}

void simulation_destroy(Simulation *const sim) {
	SDL_WaitThread(sim->thread, nullptr);
}

int32_t simulation_loop(void *data) {
	Simulation *const sim = (Simulation *)data;
	GameContext *const ctx = sim->ctx;

	while(ctx->is_window_open) {
		const uint64_t step_start_tick = SDL_GetPerformanceCounter();
		const uint64_t next_step_tick = step_start_tick + (uint64_t)(SECONDS_PER_STEP * SDL_GetPerformanceFrequency());

		memset(sim->update_map, 0, sizeof(sim->update_map));

		SDL_LockMutex(ctx->cells_mutex);
		memcpy(sim->new_cells, ctx->cells, sizeof(Cells));

		if(sim->queue_clear_cells) {
			for(uint32_t y = 0; y < GRID_HEIGHT; ++y) {
				for(uint32_t x = 0; x < GRID_WIDTH; ++x) {
					grid_create_cell(sim->new_cells, (Point){ x, y }, ID_EMPTY);
				}
			}
			sim->queue_clear_cells = false;
		}

		handle_queued_action(ctx, sim->new_cells);

		if(ctx->is_paused) {
			if(sim->queue_step) {
				sim->queue_step = false;
			} else {
				goto end_step;
			}
		}

		for(PointComponent y = GRID_HEIGHT - 1; y >= 0; y--) {
			for(PointComponent i = 0; i < GRID_WIDTH; ++i) {
				PointComponent x = sim->odd_step ? (GRID_WIDTH - i - 1) : i;

				if(sim->update_map[y][x]) {
					continue;
				}

				sim->update_map[y][x] = true;

				Point point = { x, y };
				Cell *const cell = CELL_AT_POINT(sim->new_cells, point);

				if(cell->material_id == ID_EMPTY) {
					continue;
				}
				const Material *material = material_from_id(cell->material_id);

				switch(material->type) {
				case FLUID:
					simulate_fluid(sim, cell, material, point);
					break;
				case POWDER:
					simulate_powder(sim, cell, material, point);
					break;
				case GAS:
					simulate_gas(sim, cell, material, point);
					break;
				default:
					break;
				}
			}
		}

	end_step:
		memcpy(ctx->cells, sim->new_cells, sizeof(Cells));
		SDL_UnlockMutex(ctx->cells_mutex);

		const uint64_t step_end_tick = SDL_GetPerformanceCounter();

		if(step_end_tick < next_step_tick) {
			const uint32_t sleep_time = ((float)(next_step_tick - step_end_tick) / SDL_GetPerformanceFrequency()) * 1000;
			SDL_Delay(sleep_time);
		}

		ctx->performance_stats.simulation_step_time_ns = (float)(step_end_tick - step_start_tick) / SDL_GetPerformanceFrequency() * 1000000000;
		sim->odd_step = !sim->odd_step;
	}

	return 0;
}

static void simulate_gas(Simulation *const sim, Cell *const cell, const Material *const material, const Point p) {
	// TODO: Gas cells should merge with neighbouring gas cells of the same material. (remove this cell and reduce the age of the neighbour.)

	++cell->gas.age;
	if(cell->gas.age >= cell->gas.death_age) {
		grid_create_cell(sim->new_cells, p, ID_EMPTY);
		return;
	}

	if(p.y == 0) {
		return;
	}

	Point target_point = (Point){ p.x, p.y - 1 };
	const Material *const material_below = material_from_id(CELL_AT_POINT(sim->ctx->cells, target_point)->material_id);

	if(material_below == nullptr) { // TODO: Different densities etc.
		move_cell(sim, p, target_point);
		return;
	}

	int8_t offset = (int)(round(rand() / (float)RAND_MAX) * 2 - 1);
	for(uint8_t i = 0; i < 2; ++i) {
		target_point = (Point){ p.x + offset, p.y - 1 };
		const MaterialID new_point_material_id = CELL_AT_POINT(sim->ctx->cells, target_point)->material_id;
		if(p.x + offset >= 0 && p.x + offset < GRID_WIDTH && (new_point_material_id == ID_EMPTY)) {
			move_cell(sim, p, target_point);
			return;
		}

		offset *= -1;
	}
}

static void simulate_powder(Simulation *const sim, Cell *const cell, const Material *const material, const Point p) {
	if(p.y >= GRID_HEIGHT - 1) {
		return;
	}

	Point target_point = (Point){ p.x, p.y + 1 };
	const Material *const material_below = material_from_id(CELL_AT_POINT(sim->ctx->cells, target_point)->material_id);

	if(material_below == nullptr) { // TODO: Different densities etc.
		move_cell(sim, p, target_point);
		return;
	}

	int8_t offset = (int)(round(rand() / (float)RAND_MAX) * 2 - 1);
	for(uint8_t i = 0; i < 2; ++i) {
		target_point = (Point){ p.x + offset, p.y + 1 };
		const MaterialID new_point_material_id = CELL_AT_POINT(sim->ctx->cells, target_point)->material_id;
		if(p.x + offset >= 0 && p.x + offset < GRID_WIDTH && (new_point_material_id == ID_EMPTY || new_point_material_id == ID_WATER)) {
			move_cell(sim, p, target_point);
			return;
		}

		offset *= -1;
	}
}

static void simulate_fluid(Simulation *const sim, Cell *const cell, const Material *const material, const Point p) {
	if(p.y >= GRID_HEIGHT - 1) {
		return;
	}

	Point target_point = (Point){ p.x, p.y + 1 };
	const Material *const material_below = material_from_id(CELL_AT_POINT(sim->ctx->cells, target_point)->material_id);

	if(material_below == nullptr) { // TODO: Different densities etc.
		move_cell(sim, p, target_point);
		return;
	}

	int8_t offset = (int)(round(rand() / (float)RAND_MAX) * 2 - 1);
	for(int8_t y = 0; y != -1; --y) {
		target_point = (Point){ p.x + offset, p.y + y};
		const MaterialID new_point_material_id = CELL_AT_POINT(sim->ctx->cells, target_point)->material_id;

		if(p.x + offset >= 0 && p.x + offset < GRID_WIDTH && new_point_material_id == ID_EMPTY) {
			move_cell(sim, p, target_point);
			return;
		}

		offset *= -1;
	}

	for(uint8_t i = 0; i < 2; ++i) {
		target_point = (Point){ p.x + offset, p.y };
		const MaterialID new_point_material_id = CELL_AT_POINT(sim->ctx->cells, target_point)->material_id;

		if(p.x + offset >= 0 && p.x + offset < GRID_WIDTH && new_point_material_id == ID_EMPTY) {
			move_cell(sim, p, target_point);
			return;
		}

		offset *= -1;
	}
}

static void move_cell(Simulation *const sim, const Point from, const Point to) {
	grid_move_cell(sim->new_cells, from, to);
	sim->update_map[to.y][to.x] = true;
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

	grid_fill_circle(cells, (Point){ctx->hovered_x, ctx->hovered_y}, material_id, ctx->brush_size);
}
