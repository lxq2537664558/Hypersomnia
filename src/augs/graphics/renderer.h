#pragma once
#include <vector>
#include <optional>

#include "augs/math/vec2.h"

#include "augs/misc/timer.h"
#include "augs/templates/settable_as_current_mixin.h"

#include "augs/graphics/rgba.h"
#include "augs/graphics/vertex.h"
#include "augs/graphics/fbo.h"
#include "augs/graphics/texture.h"

#include "game/assets/game_image_id.h"
class assets_manager;
struct camera_cone;

namespace augs {
	class window;

	namespace graphics {
		class fbo;
		class texture;
	}

	class renderer : public settable_as_current_mixin<renderer> {
		friend class settable_as_current_base;

		bool set_as_current_impl() { return true; }
		void set_current_to_none_impl() {}
	public:
		struct debug_line {
			debug_line(vec2 a = vec2(), vec2 b = vec2(), rgba col = rgba(255, 255, 255, 255)) : col(col), a(a), b(b) {}

			rgba col;
			vec2 a;
			vec2 b;
		};

		struct line_channel {
			std::vector<debug_line> lines;

			void draw(vec2 a, vec2 b, rgba = white);

			void draw_red(vec2 a, vec2 b);
			void draw_green(vec2 a, vec2 b);
			void draw_blue(vec2 a, vec2 b);
			void draw_yellow(vec2 a, vec2 b);
			void draw_cyan(vec2 a, vec2 b);
		};

		std::optional<graphics::fbo> illuminating_smoke_fbo;
		std::optional<graphics::fbo> smoke_fbo;
		std::optional<graphics::fbo> light_fbo;

		line_channel logic_lines;
		line_channel prev_logic_lines;
		line_channel frame_lines;
		line_channel persistent_lines;
		
		timer line_timer;

		unsigned int position_buffer_id;
		unsigned int texcoord_buffer_id;
		unsigned int color_buffer_id;
		unsigned int triangle_buffer_id;
		unsigned int special_buffer_id;

		unsigned int imgui_elements_id;

		vertex_triangle_buffer triangles;
		vertex_line_buffer lines;
		special_buffer specials;

		unsigned long long triangles_drawn_total = 0;

		bool should_interpolate_debug_lines = false;

		renderer(const vec2i screen_size);

		renderer(renderer&&) = delete;
		renderer& operator=(renderer&&) = delete;

		renderer(const renderer&) = delete;
		renderer& operator=(const renderer&) = delete;

		void resize_fbos(const vec2i screen_size);

		void set_active_texture(const unsigned);

		void fullscreen_quad();
		
		void clear_logic_lines();
		void clear_frame_lines();
		
		void draw_call_imgui(const assets_manager&);

		void draw_debug_info(
			const camera_cone,
			const assets::game_image_id line_texture, 
			const double interpolation_ratio
		);

		void set_clear_color(const rgba);
		void clear_current_fbo();

		void set_standard_blending();
		void set_additive_blending();
		
		void enable_special_vertex_attribute();
		void disable_special_vertex_attribute();
		void call_triangles();
		void call_triangles(const vertex_triangle_buffer&);
		void call_lines();
		void set_viewport(xywhi);
		void push_line(const vertex_line&);
		void push_triangle(const vertex_triangle&);
		void push_triangles(const vertex_triangle_buffer&);
		void push_special_vertex_triangle(augs::special, augs::special, augs::special);

		void clear_special_vertex_data();
		void clear_triangles();
		void clear_lines();

		void call_and_clear_triangles();

		size_t get_max_texture_size() const;

		int get_triangle_count() const;
		vertex_triangle& get_triangle(int i);
		std::vector<vertex_triangle>& get_triangle_buffer();

		void clear_geometry();
	};
}
