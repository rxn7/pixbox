#include "material.h"
#include <stdio.h>

const Material SAND_MATERIAL = {.type = POWDER, .color_palette = {{{194, 178, 128}}, {{209, 190, 141}}, {{224, 203, 154}}, {{239, 216, 167}}, {{254, 229, 180}}, {{244, 220, 165}}, {{234, 211, 150}}, {{224, 202, 135}}}};
const Material WATER_MATERIAL = {.type = FLUID, .color_palette = {{{0, 105, 148}}, {{0, 105, 148}}, {{0, 105, 148}}, {{0, 105, 148}}, {{0, 105, 148}}, {{0, 105, 148}}, {{0, 105, 148}}, {{0, 105, 148}}}};
const Material FIRE_MATERIAL = {.type = UNIQUE, .color_palette = {{{205, 38, 38}}, {{255, 48, 48}}, {{255, 58, 58}}, {{255, 68, 68}}, {{255, 78, 78}}, {{235, 71, 71}}, {{215, 64, 64}}, {{195, 57, 57}}}};
const Material GAS_MATERIAL = {.type = GAS, .color_palette = {{{152, 152, 152}}, {{177, 177, 177}}, {{202, 202, 202}}, {{227, 227, 227}}, {{252, 252, 252}}, {{232, 232, 232}}, {{212, 212, 212}}, {{192, 192, 192}}}};
const Material WOOD_MATERIAL = {.type = SOLID, .color_palette = {{{101, 67, 33}}, {{118, 78, 39}}, {{135, 89, 45}}, {{152, 100, 51}}, {{169, 111, 57}}, {{156, 102, 53}}, {{143, 93, 49}}, {{130, 84, 45}}}};
const Material ACID_MATERIAL = {.type = FLUID, .color_palette = {{{57, 181, 74}}, {{67, 199, 86}}, {{77, 217, 98}}, {{87, 235, 110}}, {{97, 253, 122}}, {{89, 233, 112}}, {{81, 213, 102}}, {{73, 193, 92}}}};
const Material OIL_MATERIAL = {.type = FLUID, .color_palette = {{{19, 19, 19}}, {{22, 22, 22}}, {{25, 25, 25}}, {{28, 28, 28}}, {{31, 31, 31}}, {{29, 29, 29}}, {{27, 27, 27}}, {{25, 25, 25}}}};
const Material ICE_MATERIAL = {.type = SOLID, .color_palette = {{{188, 222, 241}}, {{209, 231, 245}}, {{230, 240, 249}}, {{251, 249, 253}}, {{255, 255, 255}}, {{235, 235, 235}}, {{215, 215, 215}}, {{195, 195, 195}}}};

const Material *material_from_id(uint8_t id) {
	switch(id) {
	case ID_EMPTY:
		return nullptr;
	case ID_SAND:
		return &SAND_MATERIAL;
	case ID_WATER:
		return &WATER_MATERIAL;
	case ID_FIRE:
		return &FIRE_MATERIAL;
	case ID_GAS:
		return &GAS_MATERIAL;
	case ID_WOOD:
		return &WOOD_MATERIAL;
	case ID_ACID:
		return &ACID_MATERIAL;
	case ID_OIL:
		return &OIL_MATERIAL;
	case ID_ICE:
		return &ICE_MATERIAL;
	default:
		fprintf(stderr, "Invalid material id: %d\n", id);
		return nullptr;
	}
}
