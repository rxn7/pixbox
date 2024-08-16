#include "../material.h"

const Material SMOKE_MATERIAL = {
	.id = ID_SMOKE,
	.name = "smoke",
	.type = GAS,
	.gas = { .min_death_age = 200, .max_death_age = 350 },
	.color_palette = { { { 169, 169, 169 } }, { { 192, 192, 192 } }, { { 211, 211, 211 } }, { { 160, 160, 160 } }, { { 105, 105, 105 } }, { { 128, 128, 128 } }, { { 169, 169, 169 } }, { { 220, 220, 220 } } }
};
