#pragma once
#include <set>
#include "game/transcendental/entity_id.h"
#include "game/enums/attitude_type.h"

#include "game/container_sizes.h"
#include "augs/misc/constant_size_vector.h"

#include "augs/pad_bytes.h"

namespace components {
	struct attitude {
		// GEN INTROSPECTOR struct components::attitude
		float maximum_divergence_angle_before_shooting = 10.0;

		unsigned parties = 0;
		unsigned hostile_parties = 0;

		augs::constant_size_vector<entity_id, SPECIFIC_HOSTILE_ENTITIES_COUNT> specific_hostile_entities;
		
		entity_id currently_attacked_visible_entity;
		attitude_type target_attitude = attitude_type::NEUTRAL;

		bool is_alert = false;
		bool last_seen_target_position_inspected = false;
		pad_bytes<2> pad;

		vec2 last_seen_target_position;
		vec2 last_seen_target_velocity;
		// END GEN INTROSPECTOR
	};
}
