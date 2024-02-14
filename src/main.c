#include "game_context.h"
#include "grid.h"
#include "rules.h"

#include <SDL.h>
#include <SDL_assert.h>
#include <SDL_mutex.h>
#include <SDL_render.h>
#include <SDL_thread.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void handle_event(GameContext *ctx, const SDL_Event *event);
void render_cells(GameContext *ctx);
bool update_hovered_cell(GameContext *ctx);

int main(int argc, const char **argv) {
    srand(time(NULL));

    const uint32_t init_result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_assert_always(init_result == 0);

    GameContext ctx = (GameContext){
        .window_open = true,
        .window = SDL_CreateWindow("Pixbox", 640, 480, SDL_WINDOW_RESIZABLE),
        .renderer = nullptr
    };

    SDL_assert_always(ctx.window != nullptr);

    ctx.renderer = SDL_CreateRenderer(ctx.window, nullptr, SDL_RENDERER_SOFTWARE | SDL_RENDERER_PRESENTVSYNC);
    SDL_assert_always(ctx.renderer != nullptr);

    SDL_SetRenderLogicalPresentation(ctx.renderer, GRID_WIDTH, GRID_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX, SDL_SCALEMODE_NEAREST);

    ctx.texture = SDL_CreateTexture(ctx.renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, GRID_WIDTH, GRID_HEIGHT);

    grid_init(ctx.cells);

    SDL_Thread *rules_thread = SDL_CreateThread(handle_rules_thread, "update", (void *)&ctx);

    while (ctx.window_open) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            handle_event(&ctx, &event);
        }

        SDL_SetRenderDrawColor(ctx.renderer, 0, 0, 0, 255);
        SDL_RenderClear(ctx.renderer);

        render_cells(&ctx);
        SDL_RenderTexture(ctx.renderer, ctx.texture, nullptr, nullptr);
            
        SDL_SetRenderDrawColor(ctx.renderer, 255, 255, 255, 255);
        SDL_RenderRect(ctx.renderer, &(SDL_FRect){.x = ctx.hovered_x, .y = ctx.hovered_y, .w = 1, .h = 1});

        SDL_RenderPresent(ctx.renderer);
    }

    SDL_DestroyTexture(ctx.texture);
    SDL_DestroyRenderer(ctx.renderer);
    SDL_DestroyWindow(ctx.window);
    SDL_WaitThread(rules_thread, nullptr);
    SDL_Quit();

    return 0;
}

void handle_event(GameContext *ctx, const SDL_Event *event) {
    switch (event->type) {
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
    case SDL_EVENT_QUIT:
        ctx->window_open = false;
        break;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        ctx->spawn_cell_queued = true;
        break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
        ctx->spawn_cell_queued = false;
        break;

    case SDL_EVENT_MOUSE_MOTION:
        update_hovered_cell(ctx);
        break;
    }
}

void render_cells(GameContext *ctx) {
    SDL_Surface *surface;
    SDL_LockTextureToSurface(ctx->texture, nullptr, &surface);
    SDL_FillSurfaceRect(surface, nullptr, 0x000000);

    uint8_t *pixels = (uint8_t *)surface->pixels;

    SDL_LockMutex(ctx->cells_mutex);

    for (uint32_t y = 0; y < GRID_HEIGHT; ++y) {
        for (uint32_t x = 0; x < GRID_WIDTH; ++x) {
            Cell *cell = &ctx->cells[x][y];

            const Material *material = material_from_id(cell->material_id);
            if (!material) {
                continue;
            }

            for(uint8_t c = 0; c < 3; ++c) {
                pixels[3 * (y * surface->w + x) + c] = material->color_palette[cell->color_idx].rgb[c];
            }
        }
    }
    SDL_UnlockMutex(ctx->cells_mutex);

    SDL_UnlockTexture(ctx->texture);
}

bool update_hovered_cell(GameContext *ctx) {
    float mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    SDL_RenderCoordinatesFromWindow(ctx->renderer, mouse_x, mouse_y, &mouse_x, &mouse_y);

    if(mouse_x < 0 || mouse_x >= GRID_WIDTH || mouse_y < 0 || mouse_y >= GRID_HEIGHT) {
        return false;
    }

    ctx->hovered_x = (uint8_t)mouse_x;
    ctx->hovered_y = (uint8_t)mouse_y;

    return true;
}
