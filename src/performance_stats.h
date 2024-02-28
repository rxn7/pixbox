#pragma once

#include <stdint.h>

typedef struct {
	uint64_t frame_count;
	uint32_t frame_time;
	_Atomic uint32_t simulation_step_time;
} PerformanceStats;
