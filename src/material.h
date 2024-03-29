#pragma once

#include "color.h"

#include <stdint.h>

typedef enum {
	ID_EMPTY = 0,
	ID_SAND,
	ID_WATER,
	ID_FIRE,
	ID_GAS,
	ID_WOOD,
	ID_ACID,
	ID_OIL,
	ID_ICE,
	MATERIAL_ID_LAST
} MaterialID;

#define MATERIAL_COUNT (MATERIAL_ID_LAST - 1)

typedef enum { UNIQUE, POWDER, SOLID, FLUID, GAS } MaterialType;

typedef struct {
	const uint8_t max_age;
} GasMaterialData;

typedef struct {
} FluidMaterialData;

typedef struct {
} SolidMaterialData;

typedef struct {
	const MaterialID id;
	const char *name;
	const MaterialType type;
	const Color color_palette[8];

	union {
		const GasMaterialData gas;
		const FluidMaterialData fluid;
		const SolidMaterialData solid;
	};
} Material;

extern const Material SAND_MATERIAL;
extern const Material WATER_MATERIAL;
extern const Material FIRE_MATERIAL;
extern const Material GAS_MATERIAL;
extern const Material WOOD_MATERIAL;
extern const Material ACID_MATERIAL;
extern const Material OIL_MATERIAL;
extern const Material ICE_MATERIAL;

extern const Material *MATERIALS[MATERIAL_COUNT];

const Material *material_from_id(const MaterialID id);

#define FOR_EACH_MATERIAL(var) for(uint8_t var = 1; var < MATERIAL_ID_LAST; var++)
