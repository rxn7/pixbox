#include "../material.h"

const Material SAND_MATERIAL = { 
	.id = ID_SAND,
	.name = "sand",
	.type = POWDER,
	.color_palette = { { { 194, 178, 128 } }, { { 209, 190, 141 } }, { { 224, 203, 154 } }, { { 239, 216, 167 } }, { { 254, 229, 180 } }, { { 244, 220, 165 } }, { { 234, 211, 150 } }, { { 224, 202, 135 } } },
	.simulate = nullptr,
};
