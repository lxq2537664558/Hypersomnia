#pragma once
#include "augs/math/vec2.h"
#include "game/components/transform_component.h"
#include "augs/misc/smooth_value_field.h"
#include "game/detail/camera_cone.h"
#include "game/transcendental/entity_handle_declaration.h"

namespace augs {
	class delta;
}

class interpolation_system;

struct world_camera_settings {
	// GEN INTROSPECTOR struct world_camera_settings
	augs::smooth_value_field_settings smooth_value_field_settings; 
	float angled_look_length = 100.f;
	float smoothing_average_factor = 0.5;
	float averages_per_sec = 25;
	bool enable_smoothing = true;
	// END GEN INTROSPECTOR
};

struct world_camera {
	camera_cone camera;
	camera_cone smoothed_camera;

	bool dont_smooth_once = false;

	components::transform last_interpolant;
	vec2 last_ortho_interpolant;

	vec2 player_position_previously_seen;
	vec2 player_position_at_previous_step;

	augs::smooth_value_field smoothing_player_pos;

	void configure_size(vec2);

	void tick(
		const interpolation_system& interp, 
		augs::delta dt,
		world_camera_settings settings,
		const_entity_handle entity_to_chase
	);

	vec2i get_camera_offset_due_to_character_crosshair(
		const const_entity_handle,
		const world_camera_settings
	) const;
};