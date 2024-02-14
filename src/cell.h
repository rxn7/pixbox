#pragma once

#include "material.h"

typedef struct {
	MaterialID material_id : 4; // 0-16
	uint8_t color_idx : 3; // 0-8
	bool updated;
} Cell;
