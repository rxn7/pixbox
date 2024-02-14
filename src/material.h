#pragma once

#include "color.h"

#include <stdint.h>

typedef enum {
	ID_EMPTY,
	ID_SAND,
	ID_WATER,
	ID_FIRE,
	ID_GAS,
	ID_WOOD,
	ID_ACID,
	ID_OIL,
	ID_ICE,
} MaterialID;

typedef enum {
	UNIQUE,
	POWDER,
	SOLID,
	FLUID,
	GAS
} MaterialType;

typedef struct {
	MaterialType type;
	Color color_palette[8];
} Material;

extern const Material SAND_MATERIAL;
extern const Material WATER_MATERIAL;
extern const Material FIRE_MATERIAL;
extern const Material GAS_MATERIAL;
extern const Material WOOD_MATERIAL;
extern const Material ACID_MATERIAL;
extern const Material OIL_MATERIAL;
extern const Material ICE_MATERIAL;

const Material *material_from_id(uint8_t id);
