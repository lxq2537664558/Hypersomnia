#pragma once
#include "game/transcendental/entity_id.h"
#include "game/enums/adverse_element_type.h"
#include "game/assets/game_image_id.h"
#include "game/detail/explosions.h"

namespace components {
	struct hand_fuse {
		// GEN INTROSPECTOR struct components::hand_fuse
		augs::stepped_timestamp when_released;
		augs::stepped_timestamp when_detonates;

		sound_effect_input unpin_sound;
		sound_effect_input throw_sound;
		// END GEN INTROSPECTOR
	};
}
