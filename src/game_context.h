#pragma once

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_render.h>

typedef struct {
	bool window_open;
	SDL_Window *window;
	SDL_Renderer *renderer;
} GameContext;
