#pragma once

#include "cell.h"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

typedef struct {
  bool window_open;
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  Cell cells[160][120];
} GameContext;
