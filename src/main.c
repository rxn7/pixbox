#include "game_context.h"

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL.h>

void handle_event(GameContext *ctx, const SDL_Event *event);
void render(GameContext *ctx);

int main(int argc, const char **argv) {
    const uint32_t init_result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_assert_always(init_result == 0);

    GameContext ctx = (GameContext){
        .window_open = true,
        .window = SDL_CreateWindow("Pixbox", 640, 480, SDL_WINDOW_RESIZABLE),
        .renderer = nullptr
    };
    SDL_assert_always(ctx.window != nullptr);

    ctx.renderer = SDL_CreateRenderer(ctx.window, nullptr, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    while(ctx.window_open) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            handle_event(&ctx, &event);
        }

        SDL_RenderClear(ctx.renderer);
        render(&ctx);
        SDL_RenderPresent(ctx.renderer);
    }
}

void handle_event(GameContext *ctx, const SDL_Event *event) {
    switch(event->type) {
        case SDL_EVENT_QUIT:
            ctx->window_open = false;
            break;
    }
}

void render(GameContext *ctx) {
}
