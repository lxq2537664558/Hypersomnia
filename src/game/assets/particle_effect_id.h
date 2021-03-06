#pragma once
#include "game/container_sizes.h"
#include "game/assets/particle_effect.h"

namespace assets {
	enum class particle_effect_id : unsigned {
		INVALID,
#if BUILD_TEST_SCENES
		HEALTH_DAMAGE_SPARKLES,
		ELECTRIC_PROJECTILE_DESTRUCTION,
		PIXEL_PROJECTILE_TRACE,
		PIXEL_MUZZLE_LEAVE_EXPLOSION,
		ROUND_ROTATING_BLOOD_STREAM,
		WANDERING_PIXELS_DIRECTED,
		WANDERING_PIXELS_SPREAD,
		CONCENTRATED_WANDERING_PIXELS,
		PIXEL_METAL_SPARKLES,
		WANDERING_SMOKE,
		MUZZLE_SMOKE,
		ENGINE_PARTICLES,
		CAST_SPARKLES,
		CAST_CHARGING,
		EXHAUSTED_SMOKE,
		THUNDER_REMNANTS,
		MISSILE_SMOKE_TRAIL,
		EXPLODING_RING_SMOKE,
		EXPLODING_RING_SPARKLES,
#endif
		COUNT = MAX_PARTICLE_EFFECT_COUNT + 1
	};
}

struct particle_effect_input {
	// GEN INTROSPECTOR struct particle_effect_input
	assets::particle_effect_id id = assets::particle_effect_id::INVALID;
	particle_effect_modifier modifier;
	// END GEN INTROSPECTOR
};