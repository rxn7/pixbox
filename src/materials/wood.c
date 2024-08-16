#include "../material.h"

const Material WOOD_MATERIAL = {
	.id = ID_WOOD,
	.name = "wood",
	.type = SOLID,
	.color_palette = { { { 101, 67, 33 } }, { { 118, 78, 39 } }, { { 135, 89, 45 } }, { { 152, 100, 51 } }, { { 169, 111, 57 } }, { { 156, 102, 53 } }, { { 143, 93, 49 } }, { { 130, 84, 45 } } },
	.simulate = nullptr,
};
