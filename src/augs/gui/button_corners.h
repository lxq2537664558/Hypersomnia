#pragma once
#include <array>
#include "augs/math/rects.h"
#include "game/assets/game_image_id.h"
#include "game/assets/assets_manager.h"
#include "button_corner_type.h"

std::string get_filename_for(const button_corner_type);

bool is_lb_complement(const button_corner_type);
bool is_button_border(const button_corner_type);
bool is_button_outside_border(const button_corner_type);
bool is_button_corner(const button_corner_type);
bool is_button_side(const button_corner_type);

struct button_corners_info {
	assets::game_image_id inside_texture = assets::game_image_id::INVALID;
	bool flip_horizontally = true;

	assets::game_image_id get_tex_for_type(button_corner_type) const;

	ltrb cornered_rc_to_internal_rc(ltrb) const;
	ltrb internal_rc_to_cornered_rc(ltrb) const;
	vec2i cornered_size_to_internal_size(vec2i) const;
	vec2i internal_size_to_cornered_size(vec2i) const;

	template <class L>
	void for_each_button_corner(const ltrb rc, L callback) const {
		const auto& manager = get_assets_manager();

		for (auto i = button_corner_type::INSIDE; i < button_corner_type::COUNT; i = static_cast<button_corner_type>(static_cast<int>(i) + 1)) {
			const auto tex_id = get_tex_for_type(i);
			const auto tex_it = manager.find(tex_id);

			if (tex_it == nullptr) {
				continue;
			}

			const auto& tex = tex_it->texture_maps[texture_map_type::DIFFUSE];

			if (!tex.exists()) {
				continue;
			}

			const vec2 s = tex.get_size();

			ltrb target_rect;
			
			if (i == button_corner_type::INSIDE) {
				target_rect = rc;
			}

			else if (i == button_corner_type::LT || i == button_corner_type::LT_BORDER || i == button_corner_type::LT_INTERNAL_BORDER) {
				target_rect.set_size(s);
				target_rect.set_position(rc.left_top() - s);
			}
			
			else if (i == button_corner_type::RT || i == button_corner_type::RT_BORDER || i == button_corner_type::RT_INTERNAL_BORDER) {
				target_rect.set_size(s);
				target_rect.set_position(rc.right_top() - vec2(0, s.y));
			}

			else if (i == button_corner_type::RB || i == button_corner_type::RB_BORDER || i == button_corner_type::RB_INTERNAL_BORDER) {
				target_rect.set_size(s);
				target_rect.set_position(rc.right_bottom());
			}
			
			else if (i == button_corner_type::LB || i == button_corner_type::LB_BORDER || i == button_corner_type::LB_INTERNAL_BORDER) {
				target_rect.set_size(s);
				target_rect.set_position(rc.left_bottom() - vec2(s.x, 0));
			}


			else if (i == button_corner_type::L || i == button_corner_type::L_BORDER) {
				target_rect.set_size({ s.x, rc.h() });
				target_rect.set_position(rc.left_top() - vec2(s.x, 0));
			}

			else if (i == button_corner_type::T || i == button_corner_type::T_BORDER) {
				target_rect.set_size({ rc.w(), s.y });
				target_rect.set_position(rc.left_top() - vec2(0, s.y));
			}

			else if (i == button_corner_type::R || i == button_corner_type::R_BORDER) {
				target_rect.set_size({ s.x, rc.h() });
				target_rect.set_position(rc.right_top());
			}

			else if (i == button_corner_type::B || i == button_corner_type::B_BORDER) {
				target_rect.set_size({ rc.w(), s.y });
				target_rect.set_position(rc.left_bottom());
			}


			else if (i == button_corner_type::LB_COMPLEMENT || i == button_corner_type::LB_COMPLEMENT_BORDER) {
				target_rect.set_size(s);

				if (flip_horizontally) {
					target_rect.set_position(rc.right_bottom());
				}
				else {
					target_rect.set_position(rc.left_bottom() - vec2(s.x, 0));
				}
			}
			else {
				ensure("Unsupported button border type" && false);
			}

			callback(i, tex_id, target_rect);
		}
	}
};