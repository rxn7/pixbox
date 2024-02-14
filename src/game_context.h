#pragma once

#include "cell.h"

#include <SDL_render.h>
#include <SDL_video.h>
#include <SDL_mutex.h>
#include <stdatomic.h>

typedef struct {
    atomic_bool window_open;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

    atomic_bool spawn_cell_queued;

    Cell *hovered_cell;
    atomic_uchar hovered_x;
    atomic_uchar hovered_y;

    Cell cells[GRID_WIDTH][GRID_HEIGHT];
    SDL_Mutex *cells_mutex;
} GameContext;
