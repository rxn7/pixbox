#include "color.h"
#include "game_context.h"
#include "grid.h"
#include "material_selector.h"
#include "render.h"
#include "simulation.h"

#include <SDL.h>
#include <SDL_assert.h>
#include <SDL_mutex.h>
#include <SDL_render.h>
#include <SDL_thread.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void start(GameContext *const ctx);
static void init(GameContext *const ctx);
static void cleanup(GameContext *const ctx);
static void handle_event(GameContext *const ctx, const SDL_Event *const event);
static void render(GameContext *const ctx);
static void render_cells(GameContext *const ctx);
static void update_hovered_cell(GameContext *const ctx);
static uint32_t process_performance_stats(uint32_t interval, void *const param);

int main(int argc, const char **argv) {
	srand(time(NULL));

#ifndef NDEBUG
	const size_t cell_array_size = GRID_WIDTH * GRID_HEIGHT * sizeof(Cell);
	printf("Grid width:			%10u\n", GRID_WIDTH);
	printf("Grid height:		%10u\n", GRID_HEIGHT);
	printf("Cell struct size:	%10lu bytes\n", sizeof(Cell));
	printf("Cell array size:	%10lu bytes\n", cell_array_size);
	printf("Framebuffer size:	%10lu bytes\n", cell_array_size);
#endif

	GameContext ctx;

	init(&ctx);
	start(&ctx);
	cleanup(&ctx);

	return 0;
}

static void start(GameContext *ctx) {
	const SDL_TimerID process_performance_stats_timer = SDL_AddTimer(1000, process_performance_stats, (void *)ctx);

	uint64_t now = SDL_GetPerformanceCounter();
	uint64_t last_frame_tick = now;
	while(ctx->is_window_open) {
		now = SDL_GetPerformanceCounter();
		ctx->performance_stats.frame_time_ns = (float)(now - last_frame_tick) / SDL_GetPerformanceFrequency() * 1000000000;
		ctx->performance_stats.frame_count++;
		last_frame_tick = now;

		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			handle_event(ctx, &event);
		}

		render(ctx);

		SDL_SetCursor(ctx->material_selector.hovered_entry != nullptr ? ctx->hover_cursor : ctx->normal_cursor);
	}

	SDL_RemoveTimer(process_performance_stats_timer);
}

static void init(GameContext *ctx) {
	const uint32_t init_result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	SDL_assert_always(init_result == 0);

	ctx->cells_mutex = SDL_CreateMutex();
	ctx->is_window_open = true;
	ctx->brush_size = 2;
	ctx->selected_material_id = ID_SAND;
	ctx->framebuffer_pixel_size_ratio = 1.0f;
	ctx->is_paused = false;

	ctx->window = SDL_CreateWindow("Pixbox", 640, 480, SDL_WINDOW_RESIZABLE);
	SDL_assert_always(ctx->window != nullptr);

	ctx->hover_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
	ctx->normal_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);

	const uint32_t ttf_init_result = TTF_Init();
	SDL_assert_always(ttf_init_result == 0);
	ctx->font = TTF_OpenFont("assets/Romulus.ttf", SELECTED_MATERIAL_TEXT_SIZE);
	SDL_assert_always(ctx->font != nullptr);

	ctx->renderer = SDL_CreateRenderer(ctx->window, nullptr, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_assert_always(ctx->renderer != nullptr);

	ctx->framebuffer = SDL_CreateTexture(ctx->renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, GRID_WIDTH, GRID_HEIGHT);
	SDL_SetTextureScaleMode(ctx->framebuffer, SDL_SCALEMODE_NEAREST);

	{
		SDL_Surface *pause_text_surface = TTF_RenderText_Solid(ctx->font, "-paused-", (SDL_Color){ 255, 255, 255, 255 });
		ctx->pause_text_rect.w = pause_text_surface->w;
		ctx->pause_text_rect.h = pause_text_surface->h;
		ctx->pause_text_texture = SDL_CreateTextureFromSurface(ctx->renderer, pause_text_surface);
		SDL_DestroySurface(pause_text_surface);
	}

	grid_init(ctx->cells);
	material_selector_init(&ctx->material_selector, ctx);

	simulation_init(&ctx->simulation, ctx);
}

static void cleanup(GameContext *ctx) {
	material_selector_destroy(&ctx->material_selector);
	simulation_destroy(&ctx->simulation);
	TTF_CloseFont(ctx->font);
	SDL_DestroyTexture(ctx->framebuffer);
	SDL_DestroyTexture(ctx->pause_text_texture);
	SDL_DestroyCursor(ctx->hover_cursor);
	SDL_DestroyCursor(ctx->normal_cursor);
	SDL_DestroyRenderer(ctx->renderer);
	SDL_DestroyWindow(ctx->window);
	SDL_DestroyMutex(ctx->cells_mutex);
	SDL_Quit();
}

static void handle_event(GameContext *ctx, const SDL_Event *event) {
	switch(event->type) {
	case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
	case SDL_EVENT_QUIT:
		ctx->is_window_open = false;
		break;

	case SDL_EVENT_MOUSE_BUTTON_DOWN:
		switch(event->button.button) {
		case SDL_BUTTON_LEFT:
			if(ctx->material_selector.hovered_entry != nullptr) {
				ctx->selected_material_id = ctx->material_selector.hovered_entry->material->id;
				ctx->material_selector.queue_text_redraw = true;
				break;
			}
			ctx->queued_action = ACTION_SPAWN;
			break;

		case SDL_BUTTON_RIGHT:
			ctx->queued_action = ACTION_ERASE;
			break;
		}
		break;

	case SDL_EVENT_MOUSE_BUTTON_UP:
		ctx->queued_action = ACTION_NONE;
		break;

	case SDL_EVENT_MOUSE_WHEEL:
		if(event->wheel.y < 0) {
			ctx->brush_size = SDL_max(1, ctx->brush_size - 4);
		} else if(event->wheel.y > 0) {
			ctx->brush_size = SDL_min(GRID_HEIGHT, ctx->brush_size + 4);
		}
		break;

	case SDL_EVENT_MOUSE_MOTION:
		SDL_GetMouseState(&ctx->mouse_x, &ctx->mouse_y);
		update_hovered_cell(ctx);
		break;

	case SDL_EVENT_KEY_DOWN:
		switch(event->key.keysym.sym) {
		case SDLK_r:
			grid_init(ctx->cells);
			ctx->simulation.queue_clear_cells = true;
			break;
		case SDLK_ESCAPE:
			ctx->is_paused = !ctx->is_paused;
			break;
		}
		break;

	case SDL_EVENT_WINDOW_RESIZED:
		ctx->window_w = event->window.data1;
		ctx->window_h = event->window.data2;

		const float framebuffer_available_height = ctx->window_h - MATERIAL_SELECTOR_HEIGHT;
		const float available_size_ratio = ctx->window_w / (float)framebuffer_available_height;
		const float grid_ratio = GRID_WIDTH / (float)GRID_HEIGHT;

		if(available_size_ratio < grid_ratio) {
			ctx->framebuffer_pixel_size_ratio = ctx->window_w / (float)GRID_WIDTH;
			ctx->framebuffer_rect.x = 0.0f;
			ctx->framebuffer_rect.y = (float)(framebuffer_available_height - GRID_HEIGHT * ctx->framebuffer_pixel_size_ratio) * 0.5f;
		} else {
			ctx->framebuffer_pixel_size_ratio = framebuffer_available_height / (float)GRID_HEIGHT;
			ctx->framebuffer_rect.x = (float)(ctx->window_w - GRID_WIDTH * ctx->framebuffer_pixel_size_ratio) * 0.5f;
			ctx->framebuffer_rect.y = 0.0f;
		}

		ctx->framebuffer_rect.w = GRID_WIDTH * ctx->framebuffer_pixel_size_ratio;
		ctx->framebuffer_rect.h = GRID_HEIGHT * ctx->framebuffer_pixel_size_ratio;

		break;
	}
}

static void render(GameContext *ctx) {
	SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
	SDL_RenderClear(ctx->renderer);

	render_cells(ctx);

	SDL_RenderTexture(ctx->renderer, ctx->framebuffer, nullptr, &ctx->framebuffer_rect);
	SDL_SetRenderDrawColor(ctx->renderer, 150, 150, 150, 255);
	SDL_RenderRect(ctx->renderer, &ctx->framebuffer_rect);

	render_cirlce(ctx->renderer, (Point){ ctx->mouse_x, ctx->mouse_y }, ctx->brush_size * ctx->framebuffer_pixel_size_ratio);

	material_selector_render(&ctx->material_selector, ctx);

	if(ctx->is_paused) {
		ctx->pause_text_rect.x = ctx->window_w * 0.5f - ctx->pause_text_rect.w * 0.5f;
		ctx->pause_text_rect.y = ctx->window_h * 0.5f - ctx->pause_text_rect.h * 0.5f;
		SDL_RenderTexture(ctx->renderer, ctx->pause_text_texture, nullptr, &ctx->pause_text_rect);
	}

	SDL_RenderPresent(ctx->renderer);
}

static void render_cells(GameContext *ctx) {
	SDL_Surface *surface;
	SDL_LockTextureToSurface(ctx->framebuffer, nullptr, &surface);
	SDL_FillSurfaceRect(surface, nullptr, 0x000000);

	uint8_t *const pixels = (uint8_t *)surface->pixels;

	SDL_LockMutex(ctx->cells_mutex);
	for(uint32_t y = 0; y < GRID_HEIGHT; ++y) {
		for(uint32_t x = 0; x < GRID_WIDTH; ++x) {
			Cell *const cell = &ctx->cells[y][x];

			if(cell->material_id == ID_EMPTY) {
				continue;
			}

			const Material *material = material_from_id(cell->material_id);
			const Color color = material->color_palette[cell->color_idx];
			for(uint8_t c = 0; c < 3; ++c) {
				pixels[3 * (y * surface->w + x) + c] = color.rgb[c];
			}
		}
	}
	SDL_UnlockMutex(ctx->cells_mutex);

	SDL_UnlockTexture(ctx->framebuffer);
}

static void update_hovered_cell(GameContext *const ctx) {
	const float x = (ctx->mouse_x - ctx->framebuffer_rect.x) / ctx->framebuffer_pixel_size_ratio;
	const float y = (ctx->mouse_y - ctx->framebuffer_rect.y) / ctx->framebuffer_pixel_size_ratio;

	if(x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) {
		ctx->hovered_cell = nullptr;
		return;
	}

	ctx->hovered_x = (PointComponent)(x);
	ctx->hovered_y = (PointComponent)(y);
	ctx->hovered_cell = CELL_AT_XY(ctx->cells, ctx->hovered_y, ctx->hovered_y);
}

static uint32_t process_performance_stats(uint32_t interval, void *const param) {
	GameContext *ctx = (GameContext *)param;
	puts("\033[0;0H\033[2J");
	puts("\n====================== PERFORMANCE STATS ======================");
	printf("FPS: %u\n", (uint32_t)(ctx->performance_stats.frame_count / (interval * 0.001f)));
	printf("Frame time: %0.3f ms\n", ctx->performance_stats.frame_time_ns * 0.000001f);
	printf("Simulation step time: %0.3f ms\n", ctx->performance_stats.simulation_step_time_ns * 0.000001f);
	printf("Target Simulation step time: %0.3f ms\n", ctx->performance_stats.target_simulation_step_times_ns * 0.000001f);
	puts("===============================================================");

	ctx->performance_stats.frame_count = 0;

	return interval;
}
