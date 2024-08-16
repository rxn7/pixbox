#include "../material.h"

const Material ACID_MATERIAL = {
	.id = ID_ACID,
	.name = "acid",
	.type = FLUID,
	.color_palette = { { { 57, 181, 74 } }, { { 67, 199, 86 } }, { { 77, 217, 98 } }, { { 87, 235, 110 } }, { { 97, 253, 122 } }, { { 89, 233, 112 } }, { { 81, 213, 102 } }, { { 73, 193, 92 } } } ,
	.simulate = nullptr,
};
