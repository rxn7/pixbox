#include "../material.h"

const Material ICE_MATERIAL = {
	.id = ID_ICE,
	.name = "ice",
	.type = SOLID,
	.color_palette = { { { 188, 222, 241 } }, { { 209, 231, 245 } }, { { 230, 240, 249 } }, { { 251, 249, 253 } }, { { 255, 255, 255 } }, { { 235, 235, 235 } }, { { 215, 215, 215 } }, { { 195, 195, 195 } } },
	.simulate = nullptr,
};

