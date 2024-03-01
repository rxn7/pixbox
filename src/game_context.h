#pragma once

#include "cell.h"
#include "material_selector.h"
#include "performance_stats.h"
#include "simulation.h"

#include <SDL3_ttf/SDL_ttf.h>
#include <SDL_mutex.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <stdatomic.h>

typedef enum { ACTION_NONE = 0, ACTION_SPAWN, ACTION_ERASE } GameAction;

typedef struct GameContext {
	Simulation simulation;

	_Atomic bool is_paused;
	_Atomic bool is_window_open;
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Cursor *hover_cursor, *normal_cursor;
	TTF_Font *font;

	SDL_Texture *pause_text_texture;
	SDL_FRect pause_text_rect;

	SDL_Texture *framebuffer;
	SDL_FRect framebuffer_rect;
	float framebuffer_pixel_size_ratio;

	_Atomic GameAction queued_action;
	_Atomic MaterialID selected_material_id;
	_Atomic uint16_t brush_size;

	Cell *hovered_cell;
	_Atomic PointComponent hovered_x, hovered_y;

	float mouse_x, mouse_y;
	int32_t window_w, window_h;

	Cells cells;
	SDL_Mutex *cells_mutex;

	MaterialSelector material_selector;
	PerformanceStats performance_stats;
} GameContext;
