#pragma once

#include <stdint.h>

typedef enum {
	EMPTY,
	SAND
} CellType;

typedef struct {
	CellType type : 4; // 16 max
} Cell;
