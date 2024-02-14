#include "rules.h"
#include "game_context.h"

#include <SDL_timer.h>
#include <stdlib.h>

bool update_powder(Cells cells, uint8_t x, uint8_t y) {
    if (y >= GRID_HEIGHT-1) {
        return false;
    }

    if (cells[x][y + 1].material_id == ID_EMPTY) {
        grid_move_cell(cells, x, y, x, y + 1);
        return true;
    }

    for (int8_t offset = -1; offset <= 1; offset += 2) {
        if (x + offset >= 0 && x + offset < GRID_WIDTH && cells[x + offset][y + 1].material_id == ID_EMPTY) {
            grid_move_cell(cells, x, y, x + offset, y + 1);
            return true;
        }
    }

    return false;
}

bool update_fluid(Cells cells, uint8_t x, uint8_t y) {
    if(update_powder(cells, x, y)) {
        return true;
    }

    for (int8_t offset = -1; offset <= 1; offset += 2) {
        if (x + offset >= 0 && x + offset < GRID_WIDTH && cells[x + offset][y].material_id == ID_EMPTY) {
            grid_move_cell(cells, x, y, x + offset, y);
            return true;
        }
    }

    return true;
}

int32_t handle_rules_thread(void *data) {
    GameContext *ctx = (GameContext *)data;

    while(ctx->window_open) {
        SDL_LockMutex(ctx->cells_mutex);

        if(ctx->spawn_cell_queued) {
            grid_spawn_cell(ctx->cells, ctx->hovered_x, ctx->hovered_y, ID_WATER);
        }

        for (uint32_t x = 0; x < GRID_WIDTH; ++x) {
            for (uint32_t y = 0; y < GRID_HEIGHT; ++y) {
                Cell *cell = &ctx->cells[x][y];
                cell->updated = false;
            }
        }

            for (uint32_t x = 0; x < GRID_WIDTH; ++x) {
                for (uint32_t y = 0; y < GRID_HEIGHT; ++y) {
                Cell *cell = &ctx->cells[x][y];
                if(cell->updated) {
                    continue;
                }

                const Material *material = material_from_id(cell->material_id);
                if (!material) {
                    continue;
                }

                switch (material->type) {
                case SOLID:
                    break;
                case FLUID:
                    update_fluid(ctx->cells, x, y);
                    break;
                case POWDER:
                    update_powder(ctx->cells, x, y);
                    break;
                case GAS:
                    break;
                default:
                    break;
                }
            }
        }
        SDL_UnlockMutex(ctx->cells_mutex);

        SDL_Delay(1000 / 60);
    }

    return 0;
}
