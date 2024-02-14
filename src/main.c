#include "game_context.h"

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void handle_event(GameContext *ctx, const SDL_Event *event);
void render_cells(GameContext *ctx);
void update_cells(GameContext *ctx);
void update_powder(GameContext *ctx, uint8_t x, uint8_t y);
void create_cells(GameContext *ctx);
void move_cell(GameContext *ctx, uint8_t from_x, uint8_t from_y, uint8_t to_x, uint8_t to_y);
void spawn_cell(GameContext *ctx, uint8_t x, uint8_t y, MaterialID material_id);
void delete_cell(GameContext *ctx, uint8_t x, uint8_t y);

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

    SDL_SetRenderLogicalPresentation(ctx.renderer, 160, 120, SDL_LOGICAL_PRESENTATION_LETTERBOX, SDL_SCALEMODE_NEAREST);

    ctx.texture = SDL_CreateTexture(ctx.renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, 160, 120);

    create_cells(&ctx);

    while(ctx.window_open) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            handle_event(&ctx, &event);
        }

        SDL_SetRenderDrawColor(ctx.renderer, 0, 0, 0, 255);
        SDL_RenderClear(ctx.renderer);
        update_cells(&ctx);
        render_cells(&ctx);
        SDL_RenderTexture(ctx.renderer, ctx.texture, nullptr, nullptr);
        SDL_RenderPresent(ctx.renderer);
    }

    SDL_DestroyTexture(ctx.texture);
    SDL_DestroyRenderer(ctx.renderer);
    SDL_DestroyWindow(ctx.window);
    SDL_Quit();

    return 0;
}

void handle_event(GameContext *ctx, const SDL_Event *event) {
    switch(event->type) {
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        case SDL_EVENT_QUIT:
            ctx->window_open = false;
            break;
    }
}

void render_cells(GameContext *ctx) {
    SDL_Surface *surface;
    SDL_LockTextureToSurface(ctx->texture, nullptr, &surface);

    // Clear the surface
    SDL_FillSurfaceRect(surface, nullptr, 0x000000);

    uint8_t *pixels = (uint8_t *)surface->pixels;

    for(uint8_t y=0; y<120; ++y) {
        for(uint8_t x=0; x<160; ++x) {
            Cell *cell = &ctx->cells[x][y];

            const Material *material = material_from_id(cell->material_id);
            if(!material) {
                continue;
            }

            for(uint8_t c=0; c<3; ++c) {
                pixels[3 * (y * surface->w + x) + c] = material->color_palette[cell->color_idx].rgb[c];
            }
        }
    }

    SDL_UnlockTexture(ctx->texture);
}

void update_cells(GameContext *ctx) {
    for(uint8_t x=0; x<160; ++x) {
        for(uint8_t y=0; y<120; ++y) {
            ctx->cells[x][y].updated = false;
        }
    }

    for(uint8_t x=0; x<160; ++x) {
        for(uint8_t y=0; y<120; ++y) {
            Cell *cell = &ctx->cells[x][y];
            if(cell->updated) {
                continue;
            }

            const Material *material = material_from_id(cell->material_id);
            if(!material) {
                continue;
            }

            switch(material->type) {
                case SOLID:
                    break;
                case FLUID:
                    break;
                case POWDER:
                    update_powder(ctx, x, y);
                    break;
                case GAS:
                    break;
                default:
                    break;
            }
        }
    }
}

void update_powder(GameContext *ctx, uint8_t x, uint8_t y) {
    if(y >= 119) {
        return;
    }

    if(ctx->cells[x][y+1].material_id == ID_EMPTY) {
        move_cell(ctx, x, y, x, y+1);
        return;
    }

    int8_t offset = 2 * (rand() % 2) * 2 - 1;
    for(int i=0; i<2; ++i) {
        if(offset+x >= 0 && offset+x < 160 && ctx->cells[offset+x][y+1].material_id == ID_EMPTY) {
            move_cell(ctx, x, y, x+offset, y+1);
            break;
        }
        offset *= -1;
    }
}

void create_cells(GameContext *ctx) {
    for(uint8_t x=0; x<160; ++x) {
        for(uint8_t y=0; y<120; ++y) {
            if(!(rand() % 4)) {
                spawn_cell(ctx, x, y, ID_SAND);
            }
        }
    }
}

void move_cell(GameContext *ctx, uint8_t from_x, uint8_t from_y, uint8_t to_x, uint8_t to_y) {
    const Cell *from = &ctx->cells[from_x][from_y];
    Cell *to = &ctx->cells[to_x][to_y];

    to->material_id = from->material_id;
    to->color_idx = from->color_idx;
    to->updated = true;

    delete_cell(ctx, from_x, from_y);
}

void spawn_cell(GameContext *ctx, uint8_t x, uint8_t y, MaterialID material_id) {
    Cell *cell = &ctx->cells[x][y];
    cell->material_id = material_id;
    cell->color_idx = rand() % 8;
}

void delete_cell(GameContext *ctx, uint8_t x, uint8_t y) {
    Cell *cell = &ctx->cells[x][y];
    cell->updated = true;
    cell->material_id = ID_EMPTY;
}
