#pragma once
#include "message.h"
#include "augs/math/vec2.h"
#include "game/detail/physics/b2Fixture_index_in_component.h"
#include "game/enums/adverse_element_type.h"
#include "augs/misc/value_meter.h"

namespace messages {
	struct damage_message : public message {
		bool inflictor_destructed = false;
		meter_value_type amount = 0;
		float request_shake_for_ms = 0.f;
		entity_id inflictor;
		vec2 impact_velocity;
		vec2 point_of_impact;
		adverse_element_type type = adverse_element_type::FORCE;

		b2Fixture_index_in_component collider_b2Fixture_index;
		b2Fixture_index_in_component subject_b2Fixture_index;
	};
}