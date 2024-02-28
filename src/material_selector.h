#pragma once

#include "material.h"

#include <SDL_render.h>

typedef struct GameContext GameContext;

typedef struct {
	MaterialID material_id;
	SDL_Texture *icon_texture;
	SDL_Texture *text_texture;
} MaterialSelectorEntry;

typedef struct {
	MaterialSelectorEntry entries[MATERIAL_COUNT];
	MaterialSelectorEntry *hovered_entry;
	uint32_t total_width;
} MaterialSelector;

void material_selector_init(MaterialSelector *selector, struct GameContext *ctx);
void material_selector_render(MaterialSelector *selector, struct GameContext *ctx);
void material_selector_destroy(MaterialSelector *selector);
