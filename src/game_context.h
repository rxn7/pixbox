#pragma once

#include "cell.h"
#include "material_selector.h"
#include "performance_stats.h"

#include <SDL_mutex.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <stdatomic.h>

typedef enum { ACTION_NONE = 0, ACTION_SPAWN, ACTION_ERASE } GameAction;

typedef struct GameContext {
	_Atomic bool is_paused;
	_Atomic bool is_window_open;
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Cursor *hover_cursor, *normal_cursor;

	SDL_Texture *framebuffer;
	SDL_FRect framebuffer_rect;
	float framebuffer_pixel_size_ratio;

	_Atomic GameAction queued_action;
	_Atomic MaterialID selected_material;
	_Atomic uint16_t brush_size;

	Cell *hovered_cell;
	_Atomic uint16_t hovered_x, hovered_y;

	float mouse_x, mouse_y;
	int32_t window_w, window_h;

	Cells cells;
	SDL_Mutex *cells_mutex;

	MaterialSelector material_selector;
	PerformanceStats performance_stats;
} GameContext;
