#pragma once

#include "material.h"

typedef struct GasData {
	uint32_t age;
	uint32_t death_age;
} GasData;

typedef struct FluidData {
} FluidData;

typedef struct SolidData {
} SolidData;

typedef struct Cell {
	MaterialID material_id : 4;// 0-16
	uint8_t color_idx : 3;	   // 0-8
	union {
		GasData gas;
		FluidData fluid;
		SolidData solid;
	};
} Cell;

typedef Cell Cells[GRID_HEIGHT][GRID_WIDTH];
