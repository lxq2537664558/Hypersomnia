#pragma once
#include "game/transcendental/entity_id.h"
#include "game/transcendental/entity_handle_declaration.h"
#include "game/enums/render_layer.h"
#include "game/detail/camera_cone.h"

struct visible_entities {
	visible_entities() = default;
	
	visible_entities(
		const camera_cone cone, 
		const cosmos& cosm
	);

	void from_camera(
		const camera_cone cone,
		const cosmos& cosm
	);

	std::vector<unversioned_entity_id> all;
	per_render_layer_t<std::vector<unversioned_entity_id>> per_layer;
};