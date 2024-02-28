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

static void handle_event(GameContext *ctx, const SDL_Event *event);
static void render(GameContext *ctx);
static void render_cells(GameContext *ctx);
static void update_hovered_cell(GameContext *ctx);

int main(int argc, const char **argv) {
	srand(time(NULL));

	const uint32_t init_result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	SDL_assert_always(init_result == 0);

	GameContext ctx = (GameContext){ .window_open = true, .window = SDL_CreateWindow("Pixbox", 640, 480, SDL_WINDOW_RESIZABLE), .renderer = nullptr, .brush_size = 2, .selected_material = ID_SAND, .framebuffer_pixel_size_ratio = 1.0f, .hover_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND), .normal_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW) };

	SDL_assert_always(ctx.window != nullptr);

	ctx.renderer = SDL_CreateRenderer(ctx.window, nullptr, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_assert_always(ctx.renderer != nullptr);

	ctx.framebuffer = SDL_CreateTexture(ctx.renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, GRID_WIDTH, GRID_HEIGHT);
	SDL_SetTextureScaleMode(ctx.framebuffer, SDL_SCALEMODE_NEAREST);

	grid_init(ctx.cells);
	material_selector_init(&ctx.material_selector, &ctx);

	SDL_Thread *simulation_thread = SDL_CreateThread(simulation_loop, "update", (void *)&ctx);

	while(ctx.window_open) {
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			handle_event(&ctx, &event);
		}

		render(&ctx);
	}

	SDL_DestroyTexture(ctx.framebuffer);
	SDL_DestroyCursor(ctx.hover_cursor);
	SDL_DestroyCursor(ctx.normal_cursor);
	SDL_DestroyRenderer(ctx.renderer);
	SDL_DestroyWindow(ctx.window);
	material_selector_destroy(&ctx.material_selector);
	SDL_WaitThread(simulation_thread, nullptr);
	SDL_Quit();

	return 0;
}

static void handle_event(GameContext *ctx, const SDL_Event *event) {
	switch(event->type) {
	case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
	case SDL_EVENT_QUIT:
		ctx->window_open = false;
		break;

	case SDL_EVENT_MOUSE_BUTTON_DOWN:
		switch(event->button.button) {
		case SDL_BUTTON_LEFT:
			if(ctx->material_selector.hovered_entry != nullptr) {
				ctx->selected_material = ctx->material_selector.hovered_entry->material_id;
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
			ctx->brush_size = SDL_max(1, ctx->brush_size - 1);
		} else if(event->wheel.y > 0) {
			ctx->brush_size = SDL_min(GRID_HEIGHT, ctx->brush_size + 1);
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
			break;
		}
		break;

	case SDL_EVENT_WINDOW_RESIZED:
		ctx->window_w = event->window.data1;
		ctx->window_h = event->window.data2;

		if(ctx->window_w < ctx->window_h) {
			ctx->framebuffer_pixel_size_ratio = ctx->window_w / (float)GRID_WIDTH;
			ctx->framebuffer_rect.x = 0.0f;
			ctx->framebuffer_rect.y = (float)(ctx->window_h - GRID_HEIGHT * ctx->framebuffer_pixel_size_ratio) * 0.5f;
		} else {
			ctx->framebuffer_pixel_size_ratio = ctx->window_h / (float)GRID_HEIGHT;
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

	SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 255, 255);
	render_cirlce(ctx->renderer, (Point){ ctx->mouse_x, ctx->mouse_y }, ctx->brush_size * ctx->framebuffer_pixel_size_ratio);

	material_selector_render(&ctx->material_selector, ctx);

	SDL_RenderPresent(ctx->renderer);
}

static void render_cells(GameContext *ctx) {
	SDL_Surface *surface;
	SDL_LockTextureToSurface(ctx->framebuffer, nullptr, &surface);
	SDL_FillSurfaceRect(surface, nullptr, 0x000000);

	uint8_t *pixels = (uint8_t *)surface->pixels;

	SDL_LockMutex(ctx->cells_mutex);
	for(uint32_t y = 0; y < GRID_HEIGHT; ++y) {
		for(uint32_t x = 0; x < GRID_WIDTH; ++x) {
			Cell *cell = &ctx->cells[y][x];

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

static void update_hovered_cell(GameContext *ctx) {
	int32_t window_w, window_h;
	SDL_GetWindowSizeInPixels(ctx->window, &window_w, &window_h);

	float x = (ctx->mouse_x - ctx->framebuffer_rect.x) / ctx->framebuffer_pixel_size_ratio;
	float y = (ctx->mouse_y - ctx->framebuffer_rect.y) / ctx->framebuffer_pixel_size_ratio;
	if(x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) {
		ctx->hovered_cell = nullptr;
		return;
	}

	ctx->hovered_x = (uint16_t)x;
	ctx->hovered_y = (uint16_t)y;
	ctx->hovered_cell = cell_at(ctx->cells, (Point){ x, y });
}
