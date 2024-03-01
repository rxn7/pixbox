#pragma once

#include "grid.h"

#include <SDL_thread.h>

typedef struct GameContext GameContext;

typedef struct {
	SDL_Thread *thread;
	GameContext *ctx;
	bool queue_clear_cells;
} Simulation;

void simulation_init(Simulation *const sim, GameContext *const ctx);
void simulation_destroy(Simulation *const sim);
