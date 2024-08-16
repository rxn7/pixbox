#include "../material.h"

const Material OIL_MATERIAL = {
	.id = ID_OIL,
	.name = "oil",
	.type = FLUID,
	.color_palette = { { { 19, 19, 19 } }, { { 22, 22, 22 } }, { { 25, 25, 25 } }, { { 28, 28, 28 } }, { { 31, 31, 31 } }, { { 29, 29, 29 } }, { { 27, 27, 27 } }, { { 25, 25, 25 } } },
	.simulate = nullptr,
};
