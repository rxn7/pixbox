#include "../material.h"

const Material WATER_MATERIAL = {
	.id = ID_WATER,
	.name = "water",
	.type = FLUID,
	.color_palette = { { { 0, 105, 148 } }, { { 0, 105, 148 } }, { { 0, 105, 148 } }, { { 0, 105, 148 } }, { { 0, 105, 148 } }, { { 0, 105, 148 } }, { { 0, 105, 148 } }, { { 0, 105, 148 } } },
	.simulate = nullptr,
};
