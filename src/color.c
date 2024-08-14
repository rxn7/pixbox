#include "color.h"
#include "SDL_stdinc.h"

Color color_darken(const Color color, const float darkness) {
	const float val = SDL_clamp(darkness, 0.0f, 1.0f);
	return (Color){
		.rgb = {
			color.rgb[0] * val,
			color.rgb[1] * val,
			color.rgb[2] * val,
		}
	};
}
