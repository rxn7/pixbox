#pragma once

#include <stdint.h>

typedef struct {
	uint64_t frame_count;
	uint64_t frame_time_ns;
	uint64_t target_simulation_step_times_ns;
	_Atomic uint64_t simulation_step_time_ns;
} PerformanceStats;
