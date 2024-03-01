#pragma once

#include "grid.h"

#include <SDL_thread.h>

typedef struct GameContext GameContext;

typedef struct {
	SDL_Thread *thread;
	GameContext *ctx;
	Cells new_cells;
	SDL_Mutex *new_cells_mutex;
} Simulation;

void simulation_init(Simulation *const sim, GameContext *const ctx);
void simulation_clear_cells(Simulation *const sim);
void simulation_destroy(Simulation *const sim);
