#pragma once
#include "augs/misc/stepped_timing.h"
#include "game/detail/spells/spell_structs.h"
#include "game/transcendental/entity_handle_declaration.h"

struct spell_logic_input;

struct electric_triad_instance {
	// GEN INTROSPECTOR struct electric_triad_instance
	augs::stepped_cooldown cast_cooldown;
	// END GEN INTROSPECTOR

	bool are_additional_conditions_for_casting_fulfilled(const const_entity_handle) const;
	void perform_logic(const spell_logic_input);
};

struct electric_triad {
	using instance = electric_triad_instance;

	// GEN INTROSPECTOR struct electric_triad
	spell_common_data common;
	spell_appearance appearance;
	entity_id missile_definition;
	// END GEN INTROSPECTOR

	unsigned get_spell_logic_duration_ms() const {
		return 0u;
	}
};