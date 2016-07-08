#include "all.h"

#include "game/entity_id.h"
#include "game/cosmos.h"

#include "game/systems/render_system.h"
#include "game/stateful_systems/gui_system.h"
#include "game/temporary_systems/dynamic_tree_system.h"
#include "game/resources/manager.h"
#include "graphics/renderer.h"

#include "math/matrix.h"

#include "3rdparty/GL/OpenGL.h"

namespace rendering_scripts {
	void standard_rendering(viewing_step& step) {
		auto& state = step.camera_state;
		auto& renderer = step.renderer;
		auto& output = renderer.triangles;
		auto& cosmos = step.cosm;
		auto& dynamic_tree = cosmos.temporary_systems.get<dynamic_tree_system>();
		auto& physics = cosmos.temporary_systems.get<physics_system>();

		step.visible_entities = cosmos[dynamic_tree.determine_visible_entities_from_camera(state, physics)];
		step.visible_per_layer = render_system().get_visible_per_layer(step.visible_entities);

		auto matrix = augs::orthographic_projection<float>(0, state.visible_world_area.x, state.visible_world_area.y, 0, 0, 1);

		auto& default_shader = *resource_manager.find(assets::program_id::DEFAULT);
		auto& default_highlight_shader = *resource_manager.find(assets::program_id::DEFAULT_HIGHLIGHT);
		auto& circular_bars_shader = *resource_manager.find(assets::program_id::CIRCULAR_BARS);
		
		default_shader.use();
		{
			auto projection_matrix_uniform = glGetUniformLocation(default_shader.id, "projection_matrix");
			glUniformMatrix4fv(projection_matrix_uniform, 1, GL_FALSE, matrix.data());
		}
		
		for (int i = render_layer::UNDER_GROUND; i > render_layer::DYNAMIC_BODY; --i) {
			render_system().draw_entities(output, step.visible_per_layer[i], state);
		}

		renderer.call_triangles();
		renderer.clear_triangles();

		default_highlight_shader.use();
		{
			auto projection_matrix_uniform = glGetUniformLocation(default_highlight_shader.id, "projection_matrix");
			glUniformMatrix4fv(projection_matrix_uniform, 1, GL_FALSE, matrix.data());
		}
		
		render_system().draw_entities(output, step.visible_per_layer[render_layer::DYNAMIC_BODY], state, true);

		renderer.call_triangles();
		renderer.clear_triangles();

		default_shader.use();

		for (int i = render_layer::DYNAMIC_BODY; i >= 0; --i) {
			render_system().draw_entities(output, step.visible_per_layer[i], state);
		}

		renderer.call_triangles();
		renderer.clear_triangles();

		//renderer.draw_debug_info(
		//	msg.state.visible_world_area, 
		//	msg.state.camera_transform, 
		//	assets::texture_id::BLANK, 
		//	render.targets, 
		//	render.view_interpolation_ratio());

		circular_bars_shader.use();
		{
			auto projection_matrix_uniform = glGetUniformLocation(circular_bars_shader.id, "projection_matrix");
			glUniformMatrix4fv(projection_matrix_uniform, 1, GL_FALSE, matrix.data());
			
			vec2 upper(0.0f, 0.0f);
			vec2 lower(1.0f, 1.0f);
			(*assets::texture_id::HUD_CIRCULAR_BAR_MEDIUM).get_uv(upper);
			(*assets::texture_id::HUD_CIRCULAR_BAR_MEDIUM).get_uv(lower);
			auto center = (upper + lower) / 2;

			glUniform2f(glGetUniformLocation(circular_bars_shader.id, "texture_center"), center.x, center.y);
		
		}
		auto& gui = cosmos.stateful_systems.get<gui_system>();

		auto textual_infos = gui.hud.draw_circular_bars_and_get_textual_info(step);

		renderer.call_triangles();
		renderer.clear_triangles();
		
		default_shader.use();

		renderer.call_triangles(textual_infos);
		
		default_highlight_shader.use();

		gui.hud.draw_pure_color_highlights(step);

		renderer.call_triangles();
		renderer.clear_triangles();

		default_shader.use();

		gui.hud.draw_vertically_flying_numbers(step);

		gui.draw_complete_gui_for_camera_rendering_request(step);

		resource_manager.find(assets::atlas_id::GAME_WORLD_ATLAS)->bind();

		renderer.call_triangles();
		renderer.clear_triangles();
	}
}