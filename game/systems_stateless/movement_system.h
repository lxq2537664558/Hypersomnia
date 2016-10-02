#pragma once

class cosmos;
class fixed_step;

class movement_system {
public:

	void set_movement_flags_from_input(fixed_step& step);
	void apply_movement_forces(cosmos& cosmos);
	void generate_movement_responses(fixed_step& step);
};