#pragma once

#include "material.h"

#include <SDL_render.h>

#define MATERIAL_ENTRY_ICON_GAP 8
#define MATERIAL_ENTRY_ICON_SIZE 64
#define MATERIAL_ENTRY_ICON_PIXEL_SIZE 16
#define MATERIAL_ENTRY_TEXT_SIZE 32
#define SELECTED_MATERIAL_ENTRY_OUTLINE 5
#define MATERIAL_SELECTOR_HEIGHT (MATERIAL_ENTRY_ICON_SIZE + MATERIAL_ENTRY_ICON_GAP + MATERIAL_ENTRY_TEXT_SIZE)

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
