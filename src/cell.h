#pragma once

#include "material.h"

typedef struct {
	uint8_t age;
} GasData;

typedef struct {
} FluidData;

typedef struct {
} SolidData;

typedef struct {
	MaterialID material_id : 4;// 0-16
	uint8_t color_idx : 3;	   // 0-8
	union {
		GasData gas;
		FluidData fluid;
		SolidData solid;
	};
} Cell;

typedef Cell Cells[GRID_HEIGHT][GRID_WIDTH];
