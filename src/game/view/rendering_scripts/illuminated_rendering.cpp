#include "all.h"

#include "game/transcendental/entity_id.h"
#include "game/transcendental/entity_handle.h"
#include "game/transcendental/cosmos.h"

#include "game/systems_stateless/render_system.h"
#include "game/systems_stateless/gui_system.h"

#include "game/detail/gui/character_gui.h"
#include "game/assets/assets_manager.h"
#include "augs/graphics/renderer.h"
#include "game/view/viewing_step.h"
#include "game/components/item_slot_transfers_component.h"
#include "game/components/render_component.h"
#include "game/view/audiovisual_state.h"

#include "augs/graphics/drawers.h"

#include "augs/math/matrix.h"

#include <imgui/imgui.h>

namespace rendering_scripts {
	void illuminated_rendering(const viewing_step step) {
		auto& renderer = step.renderer;
		auto& output = renderer.triangles;
		const auto& cosmos = step.cosm;
		const auto camera = step.camera;
		const auto controlled_entity = cosmos[step.viewed_character];
		const auto controlled_crosshair = controlled_entity.alive() ? controlled_entity[child_entity_name::CHARACTER_CROSSHAIR] : cosmos[entity_id()];
		const auto& interp = step.audiovisuals.get<interpolation_system>();
		const auto& particles = step.audiovisuals.get<particles_simulation_system>();
		const auto& wandering_pixels = step.audiovisuals.get<wandering_pixels_system>();
		const auto& exploding_rings = step.audiovisuals.get<exploding_ring_system>();
		const auto& flying_numbers = step.audiovisuals.get<flying_number_indicator_system>();
		const auto& highlights = step.audiovisuals.get<pure_color_highlight_system>();
		const auto& thunders = step.audiovisuals.get<thunder_system>();
		const auto global_time_seconds = (step.get_interpolated_total_time_passed_in_seconds());
		const auto settings = step.drawing;

		const auto matrix = augs::orthographic_projection(camera.visible_world_area);

		const auto& visible_per_layer = step.visible.per_layer;

		const auto& manager = get_assets_manager();

		const auto& default_shader = manager.at(assets::shader_program_id::DEFAULT);
		const auto& illuminated_shader = manager.at(assets::shader_program_id::DEFAULT_ILLUMINATED);
		const auto& specular_highlights_shader = manager.at(assets::shader_program_id::SPECULAR_HIGHLIGHTS);
		const auto& pure_color_highlight_shader = manager.at(assets::shader_program_id::PURE_COLOR_HIGHLIGHT);
		const auto& border_highlight_shader = pure_color_highlight_shader; // the same
		const auto& circular_bars_shader = manager.at(assets::shader_program_id::CIRCULAR_BARS);
		const auto& smoke_shader = manager.at(assets::shader_program_id::SMOKE);
		const auto& illuminating_smoke_shader = manager.at(assets::shader_program_id::ILLUMINATING_SMOKE);
		const auto& exploding_rings_shader = manager.at(assets::shader_program_id::EXPLODING_RING);
		
		default_shader.set_as_current();
		default_shader.set_projection(matrix);

		renderer.smoke_fbo->set_as_current();
		renderer.clear_current_fbo();
		renderer.set_additive_blending();

		
		{
			particles.draw(
				output,
				render_layer::DIM_SMOKES,
				camera
			);

			particles.draw(
				output,
				render_layer::ILLUMINATING_SMOKES,
				camera
			);
		}

		renderer.call_and_clear_triangles();

		renderer.illuminating_smoke_fbo->set_as_current();
		renderer.clear_current_fbo();

		{
			particles.draw(
				output,
				render_layer::ILLUMINATING_SMOKES,
				camera
			);
		}

		renderer.call_and_clear_triangles();
		
		renderer.set_standard_blending();

		augs::graphics::fbo::set_current_to_none();

		const auto& light = step.audiovisuals.get<light_system>();
		
		light.render_all_lights(renderer, matrix, step, 
			[&]() {
				if (controlled_entity.alive()) {
					draw_crosshair_lasers(
						[&](const vec2 from, const vec2 to, const rgba col) {
							if (!settings.draw_weapon_laser) {
								return;
							}

							const auto& edge_tex = manager.at(assets::game_image_id::LASER_GLOW_EDGE).texture_maps[texture_map_type::DIFFUSE];
							const vec2 edge_size = static_cast<vec2>(edge_tex.get_size());

							augs::draw_line(output, camera[from], camera[to], edge_size.y/3.f, manager.at(assets::game_image_id::LASER).texture_maps[texture_map_type::NEON], col);

							const auto edge_offset = (to - from).set_length(edge_size.x);

							augs::draw_line(output, camera[to], camera[to + edge_offset], edge_size.y / 3.f, edge_tex, col);
							augs::draw_line(output, camera[from - edge_offset], camera[from], edge_size.y / 3.f, edge_tex, col, true);
						},
						[](...){},
						interp, 
						controlled_crosshair, 
						controlled_entity
					);
				}

				draw_cast_spells_highlights(
					output,
					interp,
					camera,
					cosmos,
					global_time_seconds
				);

				renderer.set_active_texture(3);
				renderer.illuminating_smoke_fbo->get_texture().bind();
				renderer.set_active_texture(0);
				
				illuminating_smoke_shader.set_as_current();
				
				renderer.fullscreen_quad();
				
				default_shader.set_as_current();

				exploding_rings.draw_highlights_of_rings(
					output,
					camera,
					cosmos
				);
			}
		);

		illuminated_shader.set_as_current();
		illuminated_shader.set_projection(matrix);

		auto draw_layer = [&](const render_layer r, const renderable_drawing_type type = renderable_drawing_type::NORMAL) {
			render_system().draw_entities(interp, global_time_seconds, output, cosmos, visible_per_layer[r], camera, type);
		};

		draw_layer(render_layer::UNDER_GROUND);
		draw_layer(render_layer::GROUND);
		draw_layer(render_layer::ON_GROUND);
		draw_layer(render_layer::TILED_FLOOR);

		renderer.call_and_clear_triangles();

		specular_highlights_shader.set_as_current();
		specular_highlights_shader.set_projection(matrix);

		draw_layer(render_layer::TILED_FLOOR, renderable_drawing_type::SPECULAR_HIGHLIGHTS);

		renderer.call_and_clear_triangles();

		illuminated_shader.set_as_current();

		draw_layer(render_layer::ON_TILED_FLOOR);
		draw_layer(render_layer::CAR_INTERIOR);
		draw_layer(render_layer::CAR_WHEEL);

		renderer.call_and_clear_triangles();

		border_highlight_shader.set_as_current();
		border_highlight_shader.set_projection(matrix);
		
		draw_layer(render_layer::SMALL_DYNAMIC_BODY, renderable_drawing_type::BORDER_HIGHLIGHTS);
		
		renderer.call_and_clear_triangles();
		
		illuminated_shader.set_as_current();
		
		draw_layer(render_layer::DYNAMIC_BODY);
		draw_layer(render_layer::SMALL_DYNAMIC_BODY);
		
		renderer.call_and_clear_triangles();

		renderer.set_active_texture(1);
		renderer.smoke_fbo->get_texture().bind();
		renderer.set_active_texture(0);

		smoke_shader.set_as_current();

		renderer.fullscreen_quad();

		default_shader.set_as_current();
		
		draw_layer(render_layer::FLYING_BULLETS);
		draw_layer(render_layer::NEON_CAPTIONS);
		
		if (settings.draw_crosshairs) {
			draw_layer(render_layer::CROSSHAIR);
		}
		
		draw_layer(render_layer::OVER_CROSSHAIR);

		if (settings.draw_weapon_laser && controlled_entity.alive()) {
			draw_crosshair_lasers(
				[&](const vec2 from, const vec2 to, const rgba col) {
					augs::draw_line(
						renderer.lines, 
						camera[from], 
						camera[to], 
						manager.at(assets::game_image_id::LASER).texture_maps[texture_map_type::DIFFUSE],
						col
					);
				},

				[&](const vec2 from, const vec2 to) {
					augs::draw_dashed_line(
						renderer.lines,
						camera[from],
						camera[to],
						manager.at(assets::game_image_id::LASER).texture_maps[texture_map_type::DIFFUSE],
						white,
						10.f,
						40.f, 
						global_time_seconds
					);
				},

				interp, 
				controlled_crosshair, 
				controlled_entity
			);

			renderer.call_lines();
			renderer.clear_lines();
		}

		{
			particles.draw(
				output,
				render_layer::ILLUMINATING_PARTICLES,
				camera
			);
		}

		{
			wandering_pixels_system::drawing_input wandering_input(output);
			wandering_input.camera = camera;

			for (const auto e : visible_per_layer[render_layer::WANDERING_PIXELS_EFFECTS]) {
				wandering_pixels.draw_wandering_pixels_for(cosmos[e], wandering_input);
			}
		}

		renderer.call_and_clear_triangles();

		circular_bars_shader.set_as_current();
		circular_bars_shader.set_projection(matrix);

		const auto set_center_uniform = [&](const auto image_id) {
			const auto upper = manager.at(image_id).texture_maps[texture_map_type::DIFFUSE].get_atlas_space_uv({ 0.0f, 0.0f });
			const auto lower = manager.at(image_id).texture_maps[texture_map_type::DIFFUSE].get_atlas_space_uv({ 1.f, 1.f });
			const auto center = (upper + lower) / 2;

			circular_bars_shader.set_uniform("texture_center", center);
		};

		set_center_uniform(assets::game_image_id::CIRCULAR_BAR_MEDIUM);

		const auto textual_infos = draw_circular_bars_and_get_textual_info(step);

		renderer.call_and_clear_triangles();
		
		set_center_uniform(assets::game_image_id::CIRCULAR_BAR_SMALL);

		draw_hud_for_released_explosives(
			output,
			renderer.specials,
			interp,
			camera,
			cosmos,
			global_time_seconds
		);

		renderer.call_and_clear_triangles();

		default_shader.set_as_current();

		renderer.call_triangles(textual_infos);

		exploding_rings_shader.set_as_current();
		exploding_rings_shader.set_projection(matrix);

		exploding_rings.draw_rings(
			output,
			renderer.specials,
			camera,
			cosmos
		);

		renderer.call_and_clear_triangles();

		pure_color_highlight_shader.set_as_current();

		highlights.draw_highlights(
			output,
			camera,
			cosmos,
			interp
		);

		thunders.draw_thunders(
			renderer.lines,
			camera
		);

		renderer.call_triangles();
		renderer.call_lines();
		renderer.clear_triangles();
		renderer.clear_lines();

		default_shader.set_as_current();

		flying_numbers.draw_numbers(
			output, 
			camera
		);

		if (settings.draw_character_gui && controlled_entity.alive()) {
			if (controlled_entity.has<components::item_slot_transfers>()) {
				auto& gui = step.audiovisuals.get<game_gui_system>();

				gui.get_character_gui(controlled_entity).draw({
						gui,
						interp,
						controlled_entity,
						step.audiovisuals.world_hover_highlighter,
						step.hotbar,
						step.interpolation_ratio,
						step.input_information,
						step.camera,
						output
					}
				);
			}
		}

		manager.at(assets::gl_texture_id::GAME_WORLD_ATLAS).bind();

		renderer.call_and_clear_triangles();

		if (DEBUG_DRAWING.enabled) {
			renderer.draw_debug_info(
				camera,
				assets::game_image_id::BLANK,
				step.get_interpolation_ratio()
			);
		}
	}
}