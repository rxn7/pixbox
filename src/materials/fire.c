#include "../material.h"
#include "../cell.h"
#include "../simulation.h"

void simulate_fire(Simulation *const sim, Cell *const cell, const Material *const material, const Point p) {
	simulate_gas(sim, cell, material, p);
}

const Material FIRE_MATERIAL = {
	.id = ID_FIRE,
	.name = "fire",
	.type = GAS,
	.color_palette = { { { 255, 85, 0 } }, { { 255, 120, 0 } }, { { 255, 160, 0 } }, { { 255, 200, 0 } }, { { 255, 140, 0 } }, { { 255, 60, 0 } }, { { 255, 90, 0 } }, { { 255, 110, 0 } } },
	.gas = { .min_death_age = 200, .max_death_age = 350, .fade_out = false },
	.simulate = simulate_fire
};
