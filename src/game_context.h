#pragma once

#include "cell.h"

#include <SDL_mutex.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <stdatomic.h>

typedef struct {
	atomic_bool window_open;
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *framebuffer;

	atomic_bool spawn_cell_queued;

	atomic_ushort brush_size;

	Cell *hovered_cell;
	atomic_uint hovered_x;
	atomic_uint hovered_y;

	Cells cells;
	SDL_Mutex *cells_mutex;
} GameContext;
