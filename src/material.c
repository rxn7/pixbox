#include "material.h"

const Material *material_from_id(const MaterialID id) {
	switch(id) {
	case ID_SAND:
		return &SAND_MATERIAL;
	case ID_WATER:
		return &WATER_MATERIAL;
	case ID_FIRE:
		return &FIRE_MATERIAL;
	case ID_SMOKE:
		return &SMOKE_MATERIAL;
	case ID_WOOD:
		return &WOOD_MATERIAL;
	case ID_ACID:
		return &ACID_MATERIAL;
	case ID_OIL:
		return &OIL_MATERIAL;
	case ID_ICE:
		return &ICE_MATERIAL;
	default:
		return nullptr;
	}
}
