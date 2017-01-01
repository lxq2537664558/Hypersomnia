#pragma once
#include "app_ui_element_location.h"
#include "augs/gui/appearance_detector.h"
#include "augs/gui/material.h"

#include "augs/padding_byte.h"

#include "game/assets/texture_id.h"

#include "application/ui/button_corners.h"
#include "application/ui/appearing_text.h"

class dx_button : public app_ui_rect_node {
public:
	augs::gui::appearance_detector detector;
	augs::gui::text::fstr caption;
	appearing_text appearing_caption;
	button_corners_info corners;
	button_corners_info border_corners;
	float elapsed_hover_time_ms = 0.f;

	float hover_highlight_maximum_distance = 8.f;
	float hover_highlight_duration_ms = 400.f;

	rgba colorize;
	bool click_callback_required = false;
	padding_byte pad[3];

	typedef app_ui_rect_node base;
	typedef base::gui_entropy gui_entropy;

	dx_button();
	vec2i get_target_button_size() const;
	void set_appearing_caption(const augs::gui::text::fstr text);
		
	template <class C, class D>
	static void advance_elements(C context, const D& this_id, const gui_entropy& entropies, const augs::delta dt) {
		for (const auto& info : entropies.get_events_for(this_id)) {
			this_id->detector.update_appearance(info);

			if (info.msg == gui_event::lclick) {
				this_id->click_callback_required = true;
			}
			if (info.msg == gui_event::hover) {
				this_id->elapsed_hover_time_ms = 0.f;
			}
		}

		if (this_id->detector.is_hovered) {
			this_id->elapsed_hover_time_ms += dt.in_milliseconds();
		}
	}

	template <class C, class D>
	static void draw(C context, const D& this_id, augs::gui::draw_info in) {
		if (!this_id->get_flag(augs::gui::flag::ENABLE_DRAWING)) {
			return;
		}

		const auto& rect_world = context.get_rect_world();
		const auto& this_tree_entry = context.get_tree_entry(this_id);

		const auto& detector = this_id->detector;

		rgba inside_col = this_id->colorize;
		rgba border_col = this_id->colorize;

		inside_col.a = 20;
		border_col.a = 190;

		if (detector.is_hovered) {
			inside_col.a = 30;
			border_col.a = 220;
		}

		const bool pushed = detector.current_appearance == augs::gui::appearance_detector::appearance::pushed;

		if (pushed) {
			inside_col.a = 60;
			border_col.a = 255;

			this_id->elapsed_hover_time_ms = this_id->hover_highlight_duration_ms;
		}

		const auto inside_mat = augs::gui::material(assets::texture_id::HOTBAR_BUTTON_INSIDE, inside_col);

		const auto internal_rc = this_id->corners.cornered_rc_to_internal_rc(this_id->rc);

		augs::gui::draw_clipped_rectangle(inside_mat, internal_rc, {}, in.v);
		
		{
			this_id->corners.for_each_button_corner(internal_rc, [&](const button_corner_type type, const assets::texture_id id, const ltrb drawn_rc) {
				if (type != button_corner_type::LB_COMPLEMENT) {
					augs::gui::draw_clipped_rectangle(augs::gui::material(id, inside_col), drawn_rc, {}, in.v, true);
				}
			});

			this_id->border_corners.for_each_button_corner(internal_rc, [&](const button_corner_type type, const assets::texture_id id, const ltrb drawn_rc) {
				if (type != button_corner_type::LB_COMPLEMENT) {
					augs::gui::draw_clipped_rectangle(augs::gui::material(id, border_col), drawn_rc, {}, in.v, true);
				}
			});

			if (this_id->detector.is_hovered) {
				auto hover_effect_rc = internal_rc;

				if (pushed) {
					const auto distance = 4.f;
					hover_effect_rc.expand_from_center(vec2(distance, distance));

					this_id->border_corners.for_each_button_corner(hover_effect_rc, [&](const button_corner_type type, const assets::texture_id id, const ltrb drawn_rc) {
						if (type != button_corner_type::LB_COMPLEMENT) {
							augs::gui::draw_clipped_rectangle(augs::gui::material(id, this_id->colorize), drawn_rc, {}, in.v, true);
						}
					});
				}
				else {
					const auto max_duration = this_id->hover_highlight_duration_ms;
					const auto max_distance = this_id->hover_highlight_maximum_distance;

					const auto distance = (1.f - std::min(max_duration, this_id->elapsed_hover_time_ms) / max_duration) * max_distance;
					hover_effect_rc.expand_from_center(vec2(distance, distance));

					this_id->border_corners.for_each_button_corner(hover_effect_rc, [&](const button_corner_type type, const assets::texture_id id, const ltrb drawn_rc) {
						if (type != button_corner_type::LB_COMPLEMENT && is_button_corner(type)) {
							augs::gui::draw_clipped_rectangle(augs::gui::material(id, this_id->colorize), drawn_rc, {}, in.v, true);
						}
					});
				}
			}
		}

		this_id->appearing_caption.target_pos = internal_rc.left_top();
		this_id->appearing_caption.draw(in.v);
	}
};