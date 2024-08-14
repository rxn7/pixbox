#pragma once

#include <stdint.h>

typedef struct {
	uint8_t rgb[3];
} Color;

Color color_darken(const Color color, const float darkness); // 1.0 means completely dark, 0 means normal
