#pragma once

#include "grid.h"
#include <SDL_thread.h>

typedef struct GameContext GameContext;

typedef struct {
	SDL_Thread *thread;
	GameContext *ctx;
	bool queue_clear_cells;
	bool queue_step;
	Cells new_cells;
	bool odd_step;
	bool update_map[GRID_HEIGHT][GRID_WIDTH];
} Simulation;

void simulation_init(Simulation *const sim, GameContext *const ctx);
void simulation_destroy(Simulation *const sim);
