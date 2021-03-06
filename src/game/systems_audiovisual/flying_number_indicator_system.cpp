#include "flying_number_indicator_system.h"
#include "augs/templates/container_templates.h"
#include "game/detail/camera_cone.h"

void flying_number_indicator_system::add(const number::input new_in) {
	number new_number;
	new_number.in = new_in;
	new_number.time_of_occurence_seconds = global_time_seconds;

	numbers.push_back(new_number);
}

void flying_number_indicator_system::advance(const augs::delta dt) {
	global_time_seconds += dt.in_seconds();

	erase_if(
		numbers,
		[this](const number& n) {
			return (global_time_seconds - n.time_of_occurence_seconds) > n.in.maximum_duration_seconds;
		}
	);
}

void flying_number_indicator_system::draw_numbers(
	augs::vertex_triangle_buffer& triangles,
	const camera_cone camera
) const {
	for (const auto& r : numbers) {
		const auto passed = global_time_seconds - r.time_of_occurence_seconds;
		const auto ratio = passed / r.in.maximum_duration_seconds;

		if (!r.first_camera_space_pos) {
			r.first_camera_space_pos = camera[r.in.pos];
		}

		auto text = r.in.text;

		text.pos = r.first_camera_space_pos.value() + vec2(r.in.impact_velocity).set_length(static_cast<float>(sqrt(passed)) * 50.f);
		// text.pos = camera[r.in.pos - vec2(0, static_cast<float>(sqrt(passed)) * 120.f)];

		text.draw_stroke(triangles);
		text.draw(triangles);
	}
}