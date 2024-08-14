#include "material_selector.h"
#include "game_context.h"

#include <stdlib.h>

static void render_selected_material_text(MaterialSelector *const selector, GameContext *const ctx) {
	const char *const text = material_from_id(ctx->selected_material_id)->name;
	SDL_Surface *const text_surface = TTF_RenderText_Solid(ctx->font, text, (SDL_Color){ 255, 255, 255, 255 });

	if(selector->text_texture != nullptr) {
		SDL_DestroyTexture(selector->text_texture);
	}

	selector->text_rect.w = text_surface->w;
	selector->text_rect.h = text_surface->h;
	selector->text_rect.y = SELECTED_MATERIAL_TEXT_SIZE;

	selector->text_texture = SDL_CreateTextureFromSurface(ctx->renderer, text_surface);
	SDL_DestroySurface(text_surface);
}

static void material_selector_entry_init(MaterialSelectorEntry *const entry, const MaterialID material_id, GameContext *const ctx) {
	const Material *const material = material_from_id(material_id);
	entry->material = material_from_id(material_id);

	SDL_Surface *const icon = SDL_CreateSurface(MATERIAL_ENTRY_ICON_PIXEL_SIZE, MATERIAL_ENTRY_ICON_PIXEL_SIZE, SDL_PIXELFORMAT_RGB24);
	SDL_LockSurface(icon);

	uint8_t *const pixels = (uint8_t *)icon->pixels;
	for(uint32_t y = 0; y < MATERIAL_ENTRY_ICON_PIXEL_SIZE; ++y) {
		for(uint32_t x = 0; x < MATERIAL_ENTRY_ICON_PIXEL_SIZE; ++x) {
			const Color *color = &material->color_palette[rand() % 8];
			for(uint8_t c = 0; c < 3; ++c) {
				pixels[3 * (y * icon->w + x) + c] = color->rgb[c];
			}
		}
	}
	SDL_UnlockSurface(icon);

	entry->icon_texture = SDL_CreateTextureFromSurface(ctx->renderer, icon);
	SDL_SetTextureScaleMode(entry->icon_texture, SDL_SCALEMODE_NEAREST);
	SDL_DestroySurface(icon);
}

static void material_selector_entry_destroy(MaterialSelectorEntry *const entry) {
	SDL_DestroyTexture(entry->icon_texture);
}

// NOTE: Returns true if the entry is hovered
static bool material_selector_entry_render(MaterialSelectorEntry *const entry, GameContext *const ctx, const float x) {
	const int y = ctx->window_h - MATERIAL_ENTRY_ICON_SIZE - SELECTED_MATERIAL_ENTRY_OUTLINE - SELECTED_MATERIAL_TEXT_SIZE;
	const SDL_FRect dst_rect = { .x = x, .y = y, .w = MATERIAL_ENTRY_ICON_SIZE, .h = MATERIAL_ENTRY_ICON_SIZE };

	SDL_RenderTexture(ctx->renderer, entry->icon_texture, nullptr, &dst_rect);

	const bool is_selected = ctx->selected_material_id == entry->material->id;
	if(is_selected) {
		SDL_RenderRect(ctx->renderer, &dst_rect);
	}

	const bool is_hovered = SDL_PointInRectFloat(&(SDL_FPoint){ ctx->mouse_x, ctx->mouse_y }, &dst_rect);
	return is_hovered;
}

void material_selector_init(MaterialSelector *const selector, GameContext *const ctx) {
	selector->queue_text_redraw = true;
	selector->text_texture = nullptr;
	selector->total_width = MATERIAL_ENTRY_ICON_SIZE * MATERIAL_COUNT + MATERIAL_ENTRY_ICON_GAP * (MATERIAL_COUNT - 1);

	FOR_EACH_MATERIAL(i) {
		material_selector_entry_init(&selector->entries[i - 1], i, ctx);
	}
}

void material_selector_render(MaterialSelector *const selector, GameContext *const ctx) {
	selector->hovered_entry = nullptr;

	float x = ctx->window_w * 0.5f - selector->total_width * 0.5f;

	FOR_EACH_MATERIAL(i) {
		if(material_selector_entry_render(&selector->entries[i - 1], ctx, x)) {
			selector->hovered_entry = &selector->entries[i - 1];
		}

		x += MATERIAL_ENTRY_ICON_SIZE + MATERIAL_ENTRY_ICON_GAP;
	}

	if(selector->queue_text_redraw) {
		selector->queue_text_redraw = false;
		render_selected_material_text(selector, ctx);
	}

	selector->text_rect.y = ctx->window_h - SELECTED_MATERIAL_TEXT_SIZE;
	selector->text_rect.x = ctx->window_w * 0.5f - selector->text_rect.w * 0.5f;
	SDL_RenderTexture(ctx->renderer, selector->text_texture, nullptr, &selector->text_rect);
}

void material_selector_destroy(MaterialSelector *const selector) {
	FOR_EACH_MATERIAL(i) {
		material_selector_entry_destroy(&selector->entries[i - 1]);
	}

	SDL_DestroyTexture(selector->text_texture);
}
