#pragma once
#include "message.h"
#include "augs/math/vec2.h"

namespace messages {
	struct collision_message : public message {
		entity_id collider;
		vec2 collider_impact_velocity, subject_impact_velocity, point;

		std::pair<size_t, size_t> subject_collider_and_convex_indices;

		bool one_is_sensor = false;

		bool operator<(const collision_message& b) const {
			return std::make_pair(subject, collider) < std::make_pair(b.subject, b.collider);
		}

		bool operator==(const collision_message& b) const {
			return std::make_pair(subject, collider) == std::make_pair(b.subject, b.collider);
		}

		enum class event_type {
			BEGIN_CONTACT,
			PRE_SOLVE,
			END_CONTACT
		} type;
	};
}