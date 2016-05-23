#include "augs/entity_system/world.h"
#include "game/components/physics_component.h"

#include "game/globals/filters.h"

#include "../components/physics_definition_component.h"

namespace ingredients {
	components::physics_definition& standard_dynamic_body(augs::entity_id e) {
		auto& physics_definition = *e += components::physics_definition();

		physics_definition.body.fixed_rotation = false;

		auto& info = physics_definition.new_fixture();
		info.from_renderable(e);

		info.filter = filters::dynamic_object();
		info.density = 1;

		return physics_definition;
	}

	components::physics_definition& see_through_dynamic_body(augs::entity_id e) {
		auto& physics_definition = *e += components::physics_definition();

		physics_definition.body.fixed_rotation = false;

		auto& info = physics_definition.new_fixture();
		info.from_renderable(e);

		info.filter = filters::see_through_dynamic_object();
		info.density = 1;

		return physics_definition;
	}

	components::physics_definition& standard_static_body(augs::entity_id e) {
		auto& physics_definition = *e += components::physics_definition();

		physics_definition.body.fixed_rotation = false;
		physics_definition.body.body_type = b2_staticBody;

		auto& info = physics_definition.new_fixture();
		info.from_renderable(e);

		info.filter = filters::dynamic_object();
		info.density = 1;

		return physics_definition;
	}
	
	components::physics_definition& bullet_round_physics(augs::entity_id e) {
		auto& physics_definition = *e += components::physics_definition();

		auto& body = physics_definition.body;
		body.bullet = true;
		body.angular_damping = 0.f,
		body.linear_damping = 0.f,
		body.gravity_scale = 0.f;
		body.angular_air_resistance = 0.f;
		body.fixed_rotation = false;
		body.angled_damping = false;
		
		auto& info = physics_definition.new_fixture();
		info.from_renderable(e);

		info.filter = filters::bullet();
		info.density = 1;
		info.disable_standard_collision_resolution = true;

		return physics_definition;
	}
}