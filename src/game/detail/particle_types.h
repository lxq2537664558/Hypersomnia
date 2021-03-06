#pragma once
#include "augs/math/vec2.h"
#include "game/components/sprite_component.h"
#include "game/detail/particle_types_declaration.h"
#include "augs/graphics/rgba.h"
#include "game/transcendental/entity_id.h"

struct general_particle {
	// GEN INTROSPECTOR struct general_particle
	vec2 pos;
	vec2 vel;
	vec2 acc;
	assets::game_image_id image_id = assets::game_image_id::INVALID;
	rgba color;
	vec2 size;
	float rotation = 0.f;
	float rotation_speed = 0.f;
	float linear_damping = 0.f;
	float angular_damping = 0.f;
	float current_lifetime_ms = 0.f;
	float max_lifetime_ms = 0.f;
	float shrink_when_ms_remaining = 0.f;
	float unshrinking_time_ms = 0.f;

	int alpha_levels = -1;
	// END GEN INTROSPECTOR

	void integrate(const float dt);
	void draw(components::sprite::drawing_input basic_input) const;
	bool is_dead() const;

	void set_position(const vec2);
	void set_velocity(const vec2);
	void set_acceleration(const vec2);
	void multiply_size(const float);
	void set_rotation(const float);
	void set_rotation_speed(const float);
	void set_max_lifetime_ms(const float);
	void colorize(const rgba);

	void set_image(
		const assets::game_image_id,
		const rgba
	);

	void set_image(
		const assets::game_image_id,
		vec2 size,
		const rgba
	);
};

struct animated_particle {
	// GEN INTROSPECTOR struct animated_particle
	vec2 pos;
	vec2 vel;
	vec2 acc;
	
	float linear_damping = 0.f;
	float current_lifetime_ms = 0.f;

	assets::game_image_id first_face = assets::game_image_id::INVALID;
	rgba color;
	float frame_duration_ms = 0.f;
	unsigned frame_count = 0;
	// END GEN INTROSPECTOR

	void integrate(const float dt);
	void draw(components::sprite::drawing_input basic_input) const;
	bool is_dead() const;

	void set_position(const vec2);
	void set_velocity(const vec2);
	void set_acceleration(const vec2);
	void multiply_size(const float);
	void set_rotation(const float);
	void set_rotation_speed(const float);
	void set_max_lifetime_ms(const float);
	void colorize(const rgba);
};

struct homing_animated_particle {
	// GEN INTROSPECTOR struct homing_animated_particle
	vec2 pos;
	vec2 vel;
	vec2 acc;

	float linear_damping = 0.f;
	float current_lifetime_ms = 0.f;

	float homing_force = 3000.f;

	assets::game_image_id first_face = assets::game_image_id::INVALID;
	rgba color;
	float frame_duration_ms = 0.f;
	unsigned frame_count = 0;
	// END GEN INTROSPECTOR

	void integrate(
		const float dt, 
		const vec2 homing_target
	);

	void draw(components::sprite::drawing_input basic_input) const;
	bool is_dead() const;

	void set_position(const vec2);
	void set_velocity(const vec2);
	void set_acceleration(const vec2);
	void multiply_size(const float);
	void set_rotation(const float);
	void set_rotation_speed(const float);
	void set_max_lifetime_ms(const float);
	void colorize(const rgba);
};