#pragma once
#include "augs/templates/maybe_const.h"

struct rgba;
class recoil_player;
struct behaviour_tree_instance;
struct car_engine_entities;
struct convex_partitioned_collider;
struct light_value_variation;
struct light_attenuation;
struct movement_subscribtion;
struct particles_effect_input;
struct sound_effect_input;
struct friction_connection;
template <class id_type>
struct basic_inventory_slot_id;
struct inventory_item_address;
struct inventory_traversal;
template <class key>
struct basic_cosmic_entropy;
struct cosmos_flyweights_state;
class cosmos_metadata;
struct cosmos_significant_state;
class config_lua_table;
struct neon_map_stamp;
struct scripted_image_stamp;
struct texture_atlas_stamp;
struct texture_atlas_metadata;

namespace augs {
	struct sound_effect_modifier;
	struct font_glyph_metadata;
	struct font_metadata_from_file;
	struct baked_font;
	struct font_loading_input;
	struct paint_circle_midpoint_command;
	struct paint_circle_filled_command;
	struct paint_line_command;
	template <class T, int const_count>
	class constant_size_vector;
	template <class Enum, class T>
	class enum_associative_array;
	struct machine_entropy;
}

namespace components {
	struct animation;
	struct animation_response;
	struct attitude;
	struct behaviour_tree;
	struct car;
	struct child;
	struct container;
	struct crosshair;
	struct damage;
	struct driver;
	struct dynamic_tree_node;
	struct fixtures;
	struct flags;
	struct force_joint;
	struct grenade;
	struct guid;
	struct gun;
	struct interpolation;
	struct item;
	struct item_slot_transfers;
	struct light;
	struct melee;
	struct movement;
	struct name;
	struct particles_existence;
	struct particle_effect_response;
	struct physical_relations;
	struct physics;
	struct polygon;
	struct position_copying;
	struct processing;
	struct render;
	struct rotation_copying;
	struct sentience;
	struct sound_existence;
	struct sound_response;
	struct special_physics;
	struct sprite;
	struct substance;
	struct tile_layer_instance;
	struct trace;
	struct transform;
	struct trigger_collision_detector;
	struct trigger;
	struct trigger_query_detector;
	struct wandering_pixels;
}

namespace resources {
	struct particle_effect_modifier;
	struct emission;
}

namespace augs {
	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, augs::sound_effect_modifier> t,
		F f
	) {
		f(t.NVP(repetitions));
		f(t.NVP(gain));
		f(t.NVP(pitch));
		f(t.NVP(max_distance));
		f(t.NVP(reference_distance));
		f(t.NVP(fade_on_exit));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, rgba> t,
		F f
	) {
		f(t.NVP(r));
		f(t.NVP(g));
		f(t.NVP(b));
		f(t.NVP(a));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, augs::font_glyph_metadata> t,
		F f
	) {
		f(t.NVP(adv));
		f(t.NVP(bear_x));
		f(t.NVP(bear_y));
		f(t.NVP(index));
		f(t.NVP(unicode));

		f(t.NVP(size));

		f(t.NVP(kerning));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, augs::font_metadata_from_file> t,
		F f
	) {
		f(t.NVP(ascender));
		f(t.NVP(descender));

		f(t.NVP(pt));

		f(t.NVP(glyphs));
		f(t.NVP(unicode_to_glyph_index));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, augs::baked_font> t,
		F f
	) {
		f(t.NVP(meta_from_file));
		f(t.NVP(glyphs_in_atlas));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, augs::font_loading_input> t,
		F f
	) {
		f(t.NVP(path));
		f(t.NVP(characters));
		
		f(t.NVP(pt));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, augs::paint_circle_midpoint_command> t,
		F f
	) {
		f(t.NVP(radius));
		f(t.NVP(border_width));
		f(t.NVP(scale_alpha));
		f(t.NVP(constrain_angle));
		f(t.NVP(angle_start));
		f(t.NVP(angle_end));
		f(t.NVP(filling));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, augs::paint_circle_filled_command> t,
		F f
	) {
		f(t.NVP(radius));
		f(t.NVP(filling));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, augs::paint_line_command> t,
		F f
	) {
		f(t.NVP(from));
		f(t.NVP(to));
		f(t.NVP(filling));
	}

	template <bool C, class F, class T, int const_count>
	void introspect(
		maybe_const_ref_t<C, augs::constant_size_vector<T, const_count>> t,
		F f
	) {
		f(t.NVP(count));
		f(t.NVP(raw));
	}

	template <bool C, class F, class Enum, class T>
	void introspect(
		maybe_const_ref_t<C, augs::enum_associative_array<Enum, T>> t,
		F f
	) {
		f(t.NVP(is_set));
		f(t.NVP(raw));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, augs::machine_entropy> t,
		F f
	) {
		f(t.NVP(local));
		f(t.NVP(remote));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, recoil_player> t,
		F f
	) {
		f(t.NVP(offsets));
		f(t.NVP(current_offset));
		f(t.NVP(reversed));
		f(t.NVP(repeat_last_n_offsets));

		f(t.NVP(single_cooldown_duration_ms));
		f(t.NVP(remaining_cooldown_duration));
		f(t.NVP(scale));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::animation> t,
		F f
	) {
		f(t.NVP(current_animation));

		f(t.NVP(priority));
		f(t.NVP(frame_num));
		f(t.NVP(player_position_ms));
		f(t.NVP(speed_factor));

		f(t.NVP(state));
		f(t.NVP(paused_state));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::animation_response> t,
		F f
	) {
		f(t.NVP(response));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::attitude> t,
		F f
	) {
		f(t.NVP(maximum_divergence_angle_before_shooting));

		f(t.NVP(parties));
		f(t.NVP(hostile_parties));

		f(t.NVP(specific_hostile_entities));
		
		f(t.NVP(currently_attacked_visible_entity));
		f(t.NVP(target_attitude));

		f(t.NVP(is_alert));
		f(t.NVP(last_seen_target_position_inspected));

		f(t.NVP(last_seen_target_position));
		f(t.NVP(last_seen_target_velocity));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, behaviour_tree_instance> t,
		F f
	) {
		f(t.NVP(state));
		f(t.NVP(tree_id));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::behaviour_tree> t,
		F f
	) {
		f(t.NVP(concurrent_trees));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, car_engine_entities> t,
		F f
	) {
		f(t.NVP(physical));
		f(t.NVP(particles));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::car> t,
		F f
	) {
		f(t.NVP(current_driver));

		f(t.NVP(interior));

		f(t.NVP(left_wheel_trigger));
		f(t.NVP(right_wheel_trigger));

		f(t.NVP(acceleration_engine));
		f(t.NVP(deceleration_engine));

		f(t.NVP(left_engine));
		f(t.NVP(right_engine));

		f(t.NVP(engine_sound));

		f(t.NVP(accelerating));
		f(t.NVP(decelerating));
		f(t.NVP(turning_right));
		f(t.NVP(turning_left));

		f(t.NVP(hand_brake));
		
		f(t.NVP(braking_damping));
		f(t.NVP(braking_angular_damping));

		f(t.NVP(input_acceleration));

		f(t.NVP(acceleration_length));

		f(t.NVP(maximum_speed_with_static_air_resistance));
		f(t.NVP(maximum_speed_with_static_damping));
		f(t.NVP(static_air_resistance));
		f(t.NVP(dynamic_air_resistance));
		f(t.NVP(static_damping));
		f(t.NVP(dynamic_damping));

		f(t.NVP(maximum_lateral_cancellation_impulse));
		f(t.NVP(lateral_impulse_multiplier));

		f(t.NVP(angular_damping));
		f(t.NVP(angular_damping_while_hand_braking));

		f(t.NVP(minimum_speed_for_maneuverability_decrease));
		f(t.NVP(maneuverability_decrease_multiplier));

		f(t.NVP(angular_air_resistance));
		f(t.NVP(angular_air_resistance_while_hand_braking));

		f(t.NVP(speed_for_pitch_unit));

		f(t.NVP(wheel_offset));

		f(t.NVP(last_turned_on));
		f(t.NVP(last_turned_off));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::child> t,
		F f
	) {
		f(t.NVP(parent));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::container> t,
		F f
	) {
		f(t.NVP(slots));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::crosshair> t,
		F f
	) {
		f(t.NVP(orbit_mode));

		f(t.NVP(recoil_entity));

		f(t.NVP(character_entity_to_chase));
		f(t.NVP(base_offset));
		f(t.NVP(bounds_for_base_offset));

		f(t.NVP(visible_world_area));
		f(t.NVP(max_look_expand));

		f(t.NVP(rotation_offset));
		f(t.NVP(size_multiplier));
		f(t.NVP(sensitivity));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::damage> t,
		F f
	) {
		f(t.NVP(amount));

		f(t.NVP(impulse_upon_hit));

		f(t.NVP(sender));

		f(t.NVP(damage_upon_collision));
		f(t.NVP(destroy_upon_damage));
		f(t.NVP(constrain_lifetime));
		f(t.NVP(constrain_distance));

		f(t.NVP(damage_charges_before_destruction));

		f(t.NVP(custom_impact_velocity));

		f(t.NVP(damage_falloff));

		f(t.NVP(damage_falloff_starting_distance));
		f(t.NVP(minimum_amount_after_falloff));

		f(t.NVP(distance_travelled));
		f(t.NVP(max_distance));
		f(t.NVP(max_lifetime_ms));
		f(t.NVP(recoil_multiplier));

		f(t.NVP(lifetime_ms));

		f(t.NVP(homing_towards_hostile_strength));
		f(t.NVP(particular_homing_target));

		f(t.NVP(saved_point_of_impact_before_death));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::driver> t,
		F f
	) {
		f(t.NVP(owned_vehicle));
		f(t.NVP(density_multiplier_while_driving));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::dynamic_tree_node> t,
		F f
	) {
		f(t.NVP(always_visible));
		f(t.NVP(activated));
		f(t.NVP(type));


		f(t.NVP(aabb));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, convex_partitioned_collider> t,
		F f
	) {
		f(t.NVP(shape));
		f(t.NVP(material));

		f(t.NVP(collision_sound_gain_mult));

		f(t.NVP(density));
		f(t.NVP(density_multiplier));
		f(t.NVP(friction));
		f(t.NVP(restitution));

		f(t.NVP(filter));
		f(t.NVP(destructible));
		f(t.NVP(sensor));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::fixtures> t,
		F f
	) {
		f(t.NVP(colliders));
		f(t.NVP(offsets_for_created_shapes));

		f(t.NVP(activated));
		f(t.NVP(is_friction_ground));
		f(t.NVP(disable_standard_collision_resolution));
		f(t.NVP(can_driver_shoot_through));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::flags> t,
		F f
	) {
		f(t.NVP(bit_flags));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::force_joint> t,
		F f
	) {
		f(t.NVP(chased_entity));

		f(t.NVP(force_towards_chased_entity));
		f(t.NVP(distance_when_force_easing_starts));
		f(t.NVP(power_of_force_easing_multiplier));

		f(t.NVP(percent_applied_to_chased_entity));

		f(t.NVP(divide_transform_mode));
		f(t.NVP(consider_rotation));

		f(t.NVP(chased_entity_offset));

		f(t.NVP(force_offsets));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::grenade> t,
		F f
	) {
		f(t.NVP(spoon));
		f(t.NVP(type));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::guid> t,
		F f
	) {
		f(t.NVP(value));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::gun> t,
		F f
	) {
		f(t.NVP(shot_cooldown));
		f(t.NVP(action_mode));
		f(t.NVP(num_last_bullets_to_trigger_low_ammo_cue));

		f(t.NVP(muzzle_velocity));

		f(t.NVP(damage_multiplier));

		f(t.NVP(bullet_spawn_offset));

		f(t.NVP(camera_shake_radius));
		f(t.NVP(camera_shake_spread_degrees));

		f(t.NVP(trigger_pressed));

		f(t.NVP(shell_velocity));
		f(t.NVP(shell_angular_velocity));

		f(t.NVP(shell_spread_degrees));

		f(t.NVP(recoil));

		f(t.NVP(shell_spawn_offset));

		f(t.NVP(magic_missile_definition));

		f(t.NVP(current_heat));
		f(t.NVP(gunshot_adds_heat));
		f(t.NVP(maximum_heat));
		f(t.NVP(engine_sound_strength));

		f(t.NVP(firing_engine_sound));
		f(t.NVP(muzzle_particles));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::interpolation> t,
		F f
	) {
		f(t.NVP(base_exponent));
		f(t.NVP(place_of_birth));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::item> t,
		F f
	) {
		f(t.NVP(current_mounting));
		f(t.NVP(intended_mounting));

		f(t.NVP(categories_for_slot_compatibility));

		f(t.NVP(charges));
		f(t.NVP(space_occupied_per_charge));
		f(t.NVP(stackable));

		f(t.NVP(dual_wield_accuracy_loss_percentage));
		f(t.NVP(dual_wield_accuracy_loss_multiplier));

		f(t.NVP(current_slot));
		f(t.NVP(target_slot_after_unmount));

		f(t.NVP(montage_time_ms));
		f(t.NVP(montage_time_left_ms));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::item_slot_transfers> t,
		F f
	) {
		f(t.NVP(pickup_timeout));
		f(t.NVP(mounting));

		f(t.NVP(only_pick_these_items));
		f(t.NVP(pick_all_touched_items_if_list_to_pick_empty));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, light_value_variation> t,
		F f
	) {
		f(t.NVP(min_value));
		f(t.NVP(max_value));
		f(t.NVP(change_speed));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, light_attenuation> t,
		F f
	) {
		f(t.NVP(base_value));
		f(t.NVP(variation));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::light> t,
		F f
	) {
		f(t.NVP(color));

		f(t.NVP(constant));
		f(t.NVP(linear));
		f(t.NVP(quadratic));
		f(t.NVP(max_distance));
		
		f(t.NVP(wall_constant));
		f(t.NVP(wall_linear));
		f(t.NVP(wall_quadratic));
		f(t.NVP(wall_max_distance));

		f(t.NVP(position_variations));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::melee> t,
		F f
	) {
		f(t.NVP(primary_move_flag));
		f(t.NVP(secondary_move_flag));
		f(t.NVP(tertiary_move_flag));

		f(t.NVP(current_state));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, movement_subscribtion> t,
		F f
	) {
		f(t.NVP(target));
		f(t.NVP(stop_response_at_zero_speed));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::movement> t,
		F f
	) {
		f(t.NVP(response_receivers));
		
		f(t.NVP(moving_left));
		f(t.NVP(moving_right));
		f(t.NVP(moving_forward));
		f(t.NVP(moving_backward));

		f(t.NVP(walking_enabled));
		f(t.NVP(enable_braking_damping));
		f(t.NVP(enable_animation));
		f(t.NVP(sprint_enabled));

		f(t.NVP(input_acceleration_axes));
		f(t.NVP(acceleration_length));

		f(t.NVP(applied_force_offset));

		f(t.NVP(non_braking_damping));
		f(t.NVP(braking_damping));

		f(t.NVP(standard_linear_damping));

		f(t.NVP(make_inert_for_ms));
		f(t.NVP(max_speed_for_movement_response));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::name> t,
		F f
	) {
		f(t.NVP(id));

		f(t.NVP(custom_nickname));
		f(t.NVP(nickname));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, particles_effect_input> t,
		F f
	) {
		f(t.NVP(effect));
		f(t.NVP(delete_entity_after_effect_lifetime));

		f(t.NVP(modifier));

		f(t.NVP(displace_source_position_within_radius));
		f(t.NVP(single_displacement_duration_ms));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::particles_existence> t,
		F f
	) {
		f(t.NVP(input));

		f(t.NVP(current_displacement));
		f(t.NVP(time_of_last_displacement));
		f(t.NVP(current_displacement_duration_bound_ms));

		f(t.NVP(time_of_birth));
		f(t.NVP(max_lifetime_in_steps));

		f(t.NVP(distribute_within_segment_of_length));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::particle_effect_response> t,
		F f
	) {
		f(t.NVP(response));
		f(t.NVP(modifier));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::physical_relations> t,
		F f
	) {
		f(t.NVP(owner_body));
		f(t.NVP(fixture_entities));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::physics> t,
		F f
	) {
		f(t.NVP(fixed_rotation));
		f(t.NVP(bullet));
		f(t.NVP(angled_damping));
		f(t.NVP(activated));

		f(t.NVP(body_type));

		f(t.NVP(angular_damping));
		f(t.NVP(linear_damping));
		f(t.NVP(linear_damping_vec));
		f(t.NVP(gravity_scale));

		f(t.NVP(transform));
		f(t.NVP(sweep));

		f(t.NVP(velocity));
		f(t.NVP(angular_velocity));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::polygon> t,
		F f
	) {
		f(t.NVP(center_neon_map));
		f(t.NVP(vertices));
		f(t.NVP(triangulation_indices));

	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::position_copying> t,
		F f
	) {
		f(t.NVP(target));

		f(t.NVP(offset));
		f(t.NVP(rotation_orbit_offset));
		
		f(t.NVP(reference_position));
		f(t.NVP(target_reference_position));
		
		f(t.NVP(scrolling_speed));

		f(t.NVP(rotation_offset));
		f(t.NVP(rotation_multiplier));

		f(t.NVP(position_copying_mode));
		f(t.NVP(position_copying_rotation));
		f(t.NVP(track_origin));
		f(t.NVP(target_newly_set));
		f(t.NVP(previous));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::processing> t,
		F f
	) {
		f(t.NVP(activated));

		f(t.NVP(processing_subject_categories));
		f(t.NVP(disabled_categories));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::render> t,
		F f
	) {
		f(t.NVP(screen_space_transform));
		f(t.NVP(draw_border));
		f(t.NVP(layer));

		f(t.NVP(border_color));

		f(t.NVP(last_step_when_visible));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::rotation_copying> t,
		F f
	) {
		f(t.NVP(target));
		f(t.NVP(stashed_target));

		f(t.NVP(easing_mode));

		f(t.NVP(colinearize_item_in_hand));
		f(t.NVP(update_value));
		
		f(t.NVP(smoothing_average_factor));
		f(t.NVP(averages_per_sec));
		
		f(t.NVP(last_rotation_interpolant));

		f(t.NVP(look_mode));
		f(t.NVP(stashed_look_mode));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::sentience> t,
		F f
	) {
		f(t.NVP(time_of_last_received_damage));
		f(t.NVP(time_of_last_exertion));

		f(t.NVP(cast_cooldown_for_all_spells));

		f(t.NVP(health));
		f(t.NVP(personal_electricity));
		f(t.NVP(consciousness));

		f(t.NVP(haste));
		f(t.NVP(electric_shield));

		f(t.NVP(spells));

		f(t.NVP(currently_casted_spell));
		f(t.NVP(transform_when_spell_casted));
		f(t.NVP(time_of_last_spell_cast));
		f(t.NVP(time_of_last_exhausted_cast));

		f(t.NVP(time_of_last_shake));
		f(t.NVP(shake_for_ms));

		f(t.NVP(comfort_zone));
		f(t.NVP(minimum_danger_amount_to_evade));
		f(t.NVP(danger_amount_from_hostile_attitude));

		f(t.NVP(aimpunch));
		f(t.NVP(health_damage_particles));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, sound_effect_input> t,
		F f
	) {
		f(t.NVP(effect));
		f(t.NVP(delete_entity_after_effect_lifetime));
		f(t.NVP(variation_number));
		f(t.NVP(direct_listener));
		f(t.NVP(modifier));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::sound_existence> t,
		F f
	) {
		f(t.NVP(input));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::sound_response> t,
		F f
	) {
		f(t.NVP(response));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, friction_connection> t,
		F f
	) {
		f(t.NVP(target));
		f(t.NVP(fixtures_connected));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::special_physics> t,
		F f
	) {
		f(t.NVP(dropped_collision_cooldown));
		f(t.NVP(owner_friction_ground));
		f(t.NVP(owner_friction_grounds));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::sprite> t,
		F f
	) {
		f(t.NVP(tex));
		f(t.NVP(color));
		f(t.NVP(size));
		f(t.NVP(size_multiplier));
		f(t.NVP(center_offset));
		f(t.NVP(rotation_offset));

		f(t.NVP(flip_horizontally));
		f(t.NVP(flip_vertically));
		
		f(t.NVP(effect));
		f(t.NVP(has_neon_map));

		f(t.NVP(max_specular_blinks));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::substance> t,
		F f
	) {
		f(t.NVP(dummy));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::tile_layer_instance> t,
		F f
	) {
		f(t.NVP(id));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::trace> t,
		F f
	) {
		f(t.NVP(max_multiplier_x));
		f(t.NVP(max_multiplier_y));

		f(t.NVP(chosen_multiplier));

		f(t.NVP(lengthening_duration_ms));
		f(t.NVP(chosen_lengthening_duration_ms));
		f(t.NVP(lengthening_time_passed_ms));

		f(t.NVP(is_it_finishing_trace));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::transform> t,
		F f
	) {
		f(t.NVP(pos));
		f(t.NVP(rotation));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::trigger_collision_detector> t,
		F f
	) {
		f(t.NVP(detection_intent_enabled));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::trigger> t,
		F f
	) {
		f(t.NVP(entity_to_be_notified));
		f(t.NVP(react_to_collision_detectors));
		f(t.NVP(react_to_query_detectors));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::trigger_query_detector> t,
		F f
	) {
		f(t.NVP(detection_intent_enabled));
		f(t.NVP(spam_trigger_requests_when_detection_intented));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, components::wandering_pixels> t,
		F f
	) {
		f(t.NVP(reach));
		f(t.NVP(face));
		f(t.NVP(count));
	}

	template <bool C, class F, class id_type>
	void introspect(
		maybe_const_ref_t<C, basic_inventory_slot_id<id_type>> t,
		F f
	) {
		f(t.NVP(type));
		f(t.NVP(container_entity));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, inventory_item_address> t,
		F f
	) {
		f(t.NVP(root_container));
		f(t.NVP(directions));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, inventory_traversal> t,
		F f
	) {
		f(t.NVP(parent_slot));
		f(t.NVP(current_address));
		f(t.NVP(attachment_offset));
		f(t.NVP(item_remains_physical));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, resources::particle_effect_modifier> t,
		F f
	) {
		f(t.NVP(colorize));
		f(t.NVP(scale_amounts));
		f(t.NVP(scale_lifetimes));
		f(t.NVP(homing_target));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, resources::emission> t,
		F f
	) {
		f(t.NVP(spread_degrees));
		f(t.NVP(base_speed));
		f(t.NVP(base_speed_variation));
		f(t.NVP(rotation_speed));
		f(t.NVP(particles_per_sec));
		f(t.NVP(stream_lifetime_ms));
		f(t.NVP(particle_lifetime_ms));
		f(t.NVP(size_multiplier));
		f(t.NVP(acceleration));
		f(t.NVP(angular_offset));
		f(t.NVP(swing_spread));
		f(t.NVP(swings_per_sec));
		f(t.NVP(min_swing_spread));
		f(t.NVP(max_swing_spread));
		f(t.NVP(min_swings_per_sec));
		f(t.NVP(max_swings_per_sec));
		f(t.NVP(swing_spread_change_rate));
		f(t.NVP(swing_speed_change_rate));
		f(t.NVP(fade_when_ms_remaining));
		f(t.NVP(num_of_particles_to_spawn_initially));

		f(t.NVP(randomize_spawn_point_within_circle_of_outer_radius));
		f(t.NVP(randomize_spawn_point_within_circle_of_inner_radius));

		f(t.NVP(starting_spawn_circle_size_multiplier));
		f(t.NVP(ending_spawn_circle_size_multiplier));

		f(t.NVP(starting_homing_force));
		f(t.NVP(ending_homing_force));

		f(t.NVP(homing_target));

		f(t.NVP(initial_rotation_variation));
		f(t.NVP(randomize_acceleration));
		f(t.NVP(should_particles_look_towards_velocity));

		f(t.NVP(particle_templates));
	}

	template <bool C, class F, class key>
	void introspect(
		maybe_const_ref_t<C, basic_cosmic_entropy<key>> t,
		F f
	) {
		f(t.NVP(cast_spells));
		f(t.NVP(intents_per_entity));
		f(t.NVP(transfer_requests));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, cosmos_flyweights_state> t,
		F f
	) {
		f(t.NVP(spells));
		f(t.NVP(collision_sound_matrix));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, cosmos_metadata> t,
		F f
	) {

		f(t.NVP(delta));
		f(t.NVP(total_steps_passed));

#if COSMOS_TRACKS_GUIDS
		f(t.NVP(next_entity_guid));
#endif
		f(t.NVP(settings));

		f(t.NVP(flyweights));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, cosmos_significant_state> t,
		F f
	) {
		f(t.NVP(meta));

		f(t.NVP(pool_for_aggregates));
		f(t.NVP(pools_for_components));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, config_lua_table> t,
		F f
	) {
		f(t.NVP(launch_mode));
		f(t.NVP(input_recording_mode));

		f(t.NVP(recording_replay_speed));

		f(t.NVP(determinism_test_cloned_cosmoi_count));

		f(t.NVP(check_content_integrity_every_launch));
		f(t.NVP(save_regenerated_atlases_as_binary));
		f(t.NVP(debug_regenerate_content_every_launch));

		f(t.NVP(enable_hrtf));
		f(t.NVP(max_number_of_sound_sources));

		f(t.NVP(audio_output_device));

		f(t.NVP(sound_effects_volume));
		f(t.NVP(music_volume));

		f(t.NVP(debug_disable_cursor_clipping));

		f(t.NVP(connect_address));
		f(t.NVP(connect_port));
		f(t.NVP(server_port));
		f(t.NVP(alternative_port));

		f(t.NVP(nickname));
		f(t.NVP(debug_second_nickname));

		f(t.NVP(mouse_sensitivity));

		f(t.NVP(tickrate));

		f(t.NVP(jitter_buffer_ms));
		f(t.NVP(client_commands_jitter_buffer_ms));

		f(t.NVP(interpolation_speed));
		f(t.NVP(misprediction_smoothing_multiplier));

		f(t.NVP(debug_var));
		f(t.NVP(debug_randomize_entropies_in_client_setup));
		f(t.NVP(debug_randomize_entropies_in_client_setup_once_every_steps));

		f(t.NVP(server_launch_http_daemon));
		f(t.NVP(server_http_daemon_port));
		f(t.NVP(server_http_daemon_html_file_path));

		f(t.NVP(db_path));
		f(t.NVP(survey_num_file_path));
		f(t.NVP(post_data_file_path));
		f(t.NVP(last_session_update_link));

		f(t.NVP(director_scenario_filename));
		f(t.NVP(menu_intro_scenario_filename));

		f(t.NVP(menu_theme_filename));

		f(t.NVP(rewind_intro_scene_by_secs));
		f(t.NVP(start_menu_music_at_secs));

		f(t.NVP(skip_credits));
		f(t.NVP(latest_news_url));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, neon_map_stamp> t,
		F f
	) {
		f(t.NVP(standard_deviation));
		f(t.NVP(radius_towards_x_axis));
		f(t.NVP(radius_towards_y_axis));
		f(t.NVP(amplification));
		f(t.NVP(alpha_multiplier));
		f(t.NVP(last_write_time_of_source));

		f(t.NVP(light_colors));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, scripted_image_stamp> t,
		F f
	) {
		f(t.NVP(commands));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, texture_atlas_stamp> t,
		F f
	) {
		f(t.NVP(image_stamps));
		f(t.NVP(font_stamps));
	}

	template <bool C, class F>
	void introspect(
		maybe_const_ref_t<C, texture_atlas_metadata> t,
		F f
	) {
		f(t.NVP(atlas_image_size));

		f(t.NVP(images));
		f(t.NVP(fonts));
	}

}