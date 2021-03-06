#pragma once
#include "augs/misc/minmax.h"
#include "game/components/transform_component.h"
#include "augs/graphics/rgba.h"
#include "augs/misc/delta.h"
#include "augs/graphics/vertex.h"
#include "game/messages/thunder_input.h"

struct camera_cone;
class particles_simulation_system;
class cosmos;

class thunder_system {
public:
	struct thunder {
		struct branch {
			std::vector<int> children;
			bool activated = true;
			bool can_have_children = true;

			float current_lifetime_ms = 0.f;
			float max_lifetime_ms = 0.f;

			vec2 from;
			vec2 to;
		};
		
		thunder_input in;

		float until_next_branching_ms = 0.f;
		unsigned num_active_branches = 0u;

		std::vector<branch> branches;

		void create_root_branch();
	};

	std::vector<thunder> thunders;

	void add(const thunder_input);

	void advance(
		const cosmos&,
		const augs::delta dt,
		particles_simulation_system& particles_output_for_effects
	);

	void draw_thunders(
		augs::vertex_line_buffer& lines,
		const camera_cone camera
	) const;

	void reserve_caches_for_entities(const size_t) const {}
};