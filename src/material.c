#include "material.h"

const Material SAND_MATERIAL = { 
	.id = ID_SAND,
	.name = "sand",
	.type = POWDER,
	.color_palette = { { { 194, 178, 128 } }, { { 209, 190, 141 } }, { { 224, 203, 154 } }, { { 239, 216, 167 } }, { { 254, 229, 180 } }, { { 244, 220, 165 } }, { { 234, 211, 150 } }, { { 224, 202, 135 } } }
};
const Material WATER_MATERIAL = {
	.id = ID_WATER,
	.name = "water",
	.type = FLUID,
	.color_palette = { { { 0, 105, 148 } }, { { 0, 105, 148 } }, { { 0, 105, 148 } }, { { 0, 105, 148 } }, { { 0, 105, 148 } }, { { 0, 105, 148 } }, { { 0, 105, 148 } }, { { 0, 105, 148 } } } 
};
const Material FIRE_MATERIAL = {
	.id = ID_FIRE,
	.name = "fire",
	.type = PLASMA,
	.color_palette = { { { 255, 85, 0 } }, { { 255, 120, 0 } }, { { 255, 160, 0 } }, { { 255, 200, 0 } }, { { 255, 140, 0 } }, { { 255, 60, 0 } }, { { 255, 90, 0 } }, { { 255, 110, 0 } } }
};
const Material SMOKE_MATERIAL = {
	.id = ID_SMOKE,
	.name = "smoke",
	.type = GAS,
	.gas = { .min_death_age = 200, .max_death_age = 350 },
	.color_palette = { { { 169, 169, 169 } }, { { 192, 192, 192 } }, { { 211, 211, 211 } }, { { 160, 160, 160 } }, { { 105, 105, 105 } }, { { 128, 128, 128 } }, { { 169, 169, 169 } }, { { 220, 220, 220 } } }
};
const Material WOOD_MATERIAL = {
	.id = ID_WOOD,
	.name = "wood",
	.type = SOLID,
	.color_palette = { { { 101, 67, 33 } }, { { 118, 78, 39 } }, { { 135, 89, 45 } }, { { 152, 100, 51 } }, { { 169, 111, 57 } }, { { 156, 102, 53 } }, { { 143, 93, 49 } }, { { 130, 84, 45 } } } 
};
const Material ACID_MATERIAL = {
	.id = ID_ACID,
	.name = "acid",
	.type = FLUID,
	.color_palette = { { { 57, 181, 74 } }, { { 67, 199, 86 } }, { { 77, 217, 98 } }, { { 87, 235, 110 } }, { { 97, 253, 122 } }, { { 89, 233, 112 } }, { { 81, 213, 102 } }, { { 73, 193, 92 } } } 
};
const Material OIL_MATERIAL = {
	.id = ID_OIL,
	.name = "oil",
	.type = FLUID,
	.color_palette = { { { 19, 19, 19 } }, { { 22, 22, 22 } }, { { 25, 25, 25 } }, { { 28, 28, 28 } }, { { 31, 31, 31 } }, { { 29, 29, 29 } }, { { 27, 27, 27 } }, { { 25, 25, 25 } } } 
};
const Material ICE_MATERIAL = {
	.id = ID_ICE,
	.name = "ice",
	.type = SOLID,
	.color_palette = { { { 188, 222, 241 } }, { { 209, 231, 245 } }, { { 230, 240, 249 } }, { { 251, 249, 253 } }, { { 255, 255, 255 } }, { { 235, 235, 235 } }, { { 215, 215, 215 } }, { { 195, 195, 195 } } } 
};

const Material *material_from_id(const MaterialID id) {
	switch(id) {
	case ID_SAND:
		return &SAND_MATERIAL;
	case ID_WATER:
		return &WATER_MATERIAL;
	case ID_FIRE:
		return &FIRE_MATERIAL;
	case ID_SMOKE:
		return &SMOKE_MATERIAL;
	case ID_WOOD:
		return &WOOD_MATERIAL;
	case ID_ACID:
		return &ACID_MATERIAL;
	case ID_OIL:
		return &OIL_MATERIAL;
	case ID_ICE:
		return &ICE_MATERIAL;
	default:
		return nullptr;
	}
}
