#pragma once
#include <vector>
#include "math/vec2.h"
#include "particle_effect_response_component.h"
#include "../resources/particle_effect.h"
#include "transform_component.h"

class physics_system;
class particles_system;

namespace resources {
	struct emission;
}

namespace shared {
	struct state_for_drawing_renderable;
}

namespace components {
	struct particle_group {
		struct stream {
			physics_system* optional_physics = nullptr;

			struct uncopyable {
				uncopyable& operator=(const uncopyable& b) { return *this; }
				std::vector<resources::particle> particles;
			} particles;

			bool destroy_after_lifetime_passed = true;
			bool stop_spawning_particles_if_chased_entity_dead = true;

			double stream_lifetime_ms = 0.0;
			double stream_max_lifetime_ms = 0.0;
			float stream_particles_to_spawn = 0.f;

			float target_spread = 0.f;

			float swing_spread = 0.f;
			float swings_per_sec = 0.f;
			float min_swing_spread = 0.f, max_swing_spread = 0.f, min_swings_per_sec = 0.f, max_swings_per_sec = 0.f;
			float swing_spread_change = 0.f, swing_speed_change = 0.f;

			float fade_when_ms_remaining = 0.f;

			resources::emission stream_info;
			bool enable_streaming = false;

			void stop_streaming() {
				enable_streaming = false;
			}
		};
		
		bool pause_emission = false;

		components::transform previous_transform;

		std::vector<stream> stream_slots;
		particle_group() { stream_slots.resize(1); }

		void draw(shared::state_for_drawing_renderable);
	private:
		friend class particles_system;
	};
}