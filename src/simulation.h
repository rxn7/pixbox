#pragma once

#include "cell.h"
#include <SDL_thread.h>

typedef struct GameContext GameContext;

typedef struct Simulation {
	SDL_Thread *thread;
	GameContext *ctx;
	bool queue_clear_cells;
	bool queue_step;
	Cells new_cells;
	bool odd_step;
	bool update_map[GRID_HEIGHT][GRID_WIDTH];
} Simulation;

typedef struct SimulateResult {
	Point point;
	Cell *cell;
} SimulateResult;

void simulation_init(Simulation *const sim, GameContext *const ctx);
void simulation_destroy(Simulation *const sim);

SimulateResult simulate_gas(Simulation *const sim, Cell *const cell, const Material *const material, const Point p);
SimulateResult simulate_gas(Simulation *const sim, Cell *const cell, const Material *const material, const Point p);
SimulateResult simulate_powder(Simulation *const sim, Cell *const cell, const Material *const material, const Point p);
SimulateResult simulate_fluid(Simulation *const sim, Cell *const cell, const Material *const material, const Point p);
