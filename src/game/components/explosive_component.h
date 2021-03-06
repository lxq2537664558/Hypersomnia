#pragma once
#include "game/transcendental/entity_id.h"
#include "game/enums/adverse_element_type.h"
#include "game/assets/game_image_id.h"
#include "game/assets/physical_material_id.h"
#include "game/detail/explosions.h"

namespace components {
	struct explosive {
		// GEN INTROSPECTOR struct components::explosive
		standard_explosion_input explosion;
		assets::game_image_id released_image_id = assets::game_image_id::INVALID;
		assets::physical_material_id released_physical_material = assets::physical_material_id::INVALID;
		// END GEN INTROSPECTOR
	};
}
